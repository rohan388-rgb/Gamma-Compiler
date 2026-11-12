from __future__ import annotations
import json
import logging
import shutil
import subprocess
import shlex
import time
from threading import Lock, Thread
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Optional

from flask import Flask, jsonify, request
from flask_cors import CORS

# =============================================================================
# SECTION 1: CONFIGURATION & CONSTANTS
# =============================================================================
# All filesystem paths are defined in one place so the rest of the codebase
# never hardcodes a path string. Two "views" of the same physical location
# are required because the compiler executable runs inside WSL while Flask
# runs on the Windows host:
#
#   - WSL_*   -> POSIX-style paths, passed as arguments to `wsl bash -lc`
#   - Local pathlib Path objects -> used by Flask to read/write/check files
#     directly, since /mnt/c/Users/... on WSL == C:\Users\... on Windows.
# -----------------------------------------------------------------------------

# ---- WSL-side (POSIX) paths, used only when building shell commands -------
WSL_PROJECT_ROOT = "/mnt/c/Users/lachy/CLionProjects/Gamma"
WSL_BUILD_DIR = f"{WSL_PROJECT_ROOT}/cmake-build-debug-wsl"
WSL_COMPILER_EXECUTABLE = f"{WSL_BUILD_DIR}/Gamma"
WSL_SOURCE_FILE = f"{WSL_PROJECT_ROOT}/backend/temp/code.hy"

# ---- Local (Windows/host) paths, used for all pathlib file operations -----
BASE_DIR = Path(__file__).resolve().parent          # .../Gamma/backend
PROJECT_ROOT = BASE_DIR.parent                        # .../Gamma
BUILD_DIR = PROJECT_ROOT / "cmake-build-debug-wsl"    # compiled artifacts live here

TEMP_DIR = BASE_DIR / "temp"
OUTPUTS_DIR = BASE_DIR / "outputs"

# Persistent Gamma source files used by the Web IDE
PROJECT_FILES_DIR = PROJECT_ROOT / "gamma_files"

SOURCE_FILE = TEMP_DIR / "code.hy"

# Compiler-generated metadata for the Web IDE
METADATA_FILE = BUILD_DIR / "compile_metadata.json"

ASSEMBLY_FILE = BUILD_DIR / "out.asm"
OBJECT_FILE = BUILD_DIR / "out.o"
EXECUTABLE_FILE = BUILD_DIR / "out"

ASSEMBLY_OUTPUT_COPY = OUTPUTS_DIR / "out.asm"

# ---- Process behaviour constants -------------------------------------------
COMPILE_TIMEOUT_SECONDS = 30
RUN_TIMEOUT_SECONDS = 10
CORS_ORIGIN = "http://localhost:5173"

# =============================================================================
# SECTION 2: LOGGING SETUP
# =============================================================================
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s | %(levelname)-8s | %(name)s | %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
)
logger = logging.getLogger("gamma_backend")

# =============================================================================
# SECTION 3: DATACLASSES
# =============================================================================
# Structured containers for subprocess results keep route handlers free of
# raw tuples/dicts and make the code self-documenting.
# -----------------------------------------------------------------------------

@dataclass
class ProcessResult:
    """Generic result of running an external process via subprocess.run()."""
    stdout: str = ""
    stderr: str = ""
    return_code: int = -1
    execution_time: float = 0.0
    timed_out: bool = False
    error_message: Optional[str] = None  # populated on Python-level exceptions


@dataclass
class CompileOutcome:
    """Aggregated result of a full /compile workflow run."""
    success: bool
    compile_time: float
    stdout: str
    stderr: str
    assembly: Optional[str]
    errors: List[str] = field(default_factory=list)


RUN_SESSIONS = {}
RUN_SESSIONS_LOCK = Lock()


# =============================================================================
# SECTION 4: HELPER FUNCTIONS
# =============================================================================

def ensure_directories() -> None:
    """Create required project directories."""
    TEMP_DIR.mkdir(parents=True, exist_ok=True)
    OUTPUTS_DIR.mkdir(parents=True, exist_ok=True)
    PROJECT_FILES_DIR.mkdir(parents=True, exist_ok=True)


def save_source(code: str) -> Path:
    """
    Persist the user's Gamma source code to backend/temp/code.hy.

    Args:
        code: Raw source code string submitted by the frontend.

    Returns:
        Path to the written source file.

    Raises:
        OSError: if the file cannot be written (propagated to caller so the
                 route handler can convert it into a proper HTTP response).
    """
    ensure_directories()
    SOURCE_FILE.write_text(code, encoding="utf-8")
    logger.info("Source code saved to %s (%d bytes)", SOURCE_FILE, len(code.encode("utf-8")))
    return SOURCE_FILE

def is_valid_gamma_filename(filename: str) -> bool:
    """
    Allow only simple .g filenames.
    """

    if not isinstance(filename, str):
        return False

    filename = filename.strip()

    if not filename:
        return False

    if Path(filename).name != filename:
        return False

    if not filename.lower().endswith(".g"):
        return False

    return True


def get_project_file_path(filename: str) -> Optional[Path]:
    """Return the safe filesystem path for a Gamma source file."""

    if not is_valid_gamma_filename(filename):
        return None

    return PROJECT_FILES_DIR / filename


def _run_wsl_command(
    shell_command: str,
    timeout: int,
    input_data: str = "",
) -> ProcessResult:
    """
    Shared low-level runner for any command that must execute inside WSL.

    input_data is forwarded to the child process through stdin.
    """
    full_command = ["wsl", "bash", "-lc", shell_command]
    logger.info("Executing WSL command: %s", shell_command)

    start_time = time.perf_counter()

    try:
        completed = subprocess.run(
            full_command,
            input=input_data,
            capture_output=True,
            text=True,
            timeout=timeout,
        )

        elapsed = time.perf_counter() - start_time

        return ProcessResult(
            stdout=completed.stdout,
            stderr=completed.stderr,
            return_code=completed.returncode,
            execution_time=elapsed,
        )

    except subprocess.TimeoutExpired as exc:
        elapsed = time.perf_counter() - start_time

        logger.error(
            "WSL command timed out after %.2fs: %s",
            elapsed,
            shell_command,
        )

        return ProcessResult(
            stdout=exc.stdout or "",
            stderr=exc.stderr or "",
            return_code=-1,
            execution_time=elapsed,
            timed_out=True,
            error_message="Process timed out.",
        )

    except FileNotFoundError:
        logger.exception("The 'wsl' executable was not found on this system.")

        return ProcessResult(
            execution_time=time.perf_counter() - start_time,
            error_message="WSL is not installed or not available on PATH.",
        )

    except Exception as exc:
        logger.exception("Unexpected error while running WSL command.")

        return ProcessResult(
            execution_time=time.perf_counter() - start_time,
            error_message=f"Unexpected error: {exc}",
        )


def run_compiler() -> ProcessResult:
    """
    Rebuild the Gamma compiler and compile the submitted source
    without running the generated program.
    """
    shell_command = (
        f"cd {WSL_BUILD_DIR} && "
        f"cmake --build . --target Gamma && "
        f"./Gamma {WSL_SOURCE_FILE} --compile-only"
    )

    return _run_wsl_command(
        shell_command,
        timeout=COMPILE_TIMEOUT_SECONDS,
    )


def _collect_program_output(run_id: str) -> None:
    """
    Continuously collect output from a running Gamma program.
    """
    with RUN_SESSIONS_LOCK:
        session = RUN_SESSIONS.get(run_id)

    if not session:
        return

    process = session["process"]

    if process.stdout is not None:
        for line in iter(process.stdout.readline, ""):
            if line:
                with RUN_SESSIONS_LOCK:
                    if run_id in RUN_SESSIONS:
                        RUN_SESSIONS[run_id]["output"] += line

            if process.poll() is not None:
                break

    process.wait()

    with RUN_SESSIONS_LOCK:
        if run_id in RUN_SESSIONS:
            RUN_SESSIONS[run_id]["finished"] = True
            RUN_SESSIONS[run_id]["exit_code"] = process.returncode
            fifo_path = RUN_SESSIONS[run_id].get("fifo_path")

    if fifo_path:
        try:
            subprocess.run(
                [
                    "wsl.exe",
                    "bash",
                    "-lc",
                    f"rm -f {shlex.quote(fifo_path)}",
                ],
                capture_output=True,
                text=True,
                timeout=5,
            )
        except Exception:
            logger.exception("Failed to clean up input FIFO.")


def start_program() -> str:
    """
    Start the compiled Gamma program without using Windows -> WSL stdin.
    A persistent WSL FIFO is used so multiple interactive inputs work.
    """
    run_id = str(time.time_ns())

    fifo_path = f"/tmp/gamma_input_{run_id}"

    shell_command = (
        f"cd {shlex.quote(WSL_BUILD_DIR)} && "
        f"rm -f {shlex.quote(fifo_path)} && "
        f"mkfifo {shlex.quote(fifo_path)} && "
        f"exec 3<> {shlex.quote(fifo_path)} && "
        f"exec ./out <&3"
    )

    process = subprocess.Popen(
        ["wsl.exe", "bash", "-lc", shell_command],
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
    )

    with RUN_SESSIONS_LOCK:
        RUN_SESSIONS[run_id] = {
            "process": process,
            "output": "",
            "finished": False,
            "exit_code": None,
            "started_at": time.perf_counter(),
            "fifo_path": fifo_path,
        }

    Thread(
        target=_collect_program_output,
        args=(run_id,),
        daemon=True,
    ).start()

    return run_id
def read_compile_metadata() -> dict:
    """
    Read token/AST metadata generated by the C++ compiler.

    Returns an empty structure if metadata is unavailable.
    """
    empty_metadata = {
        "tokens": [],
        "ast": {
            "type": "Program",
            "children": []
        }
    }

    if not METADATA_FILE.exists():
        logger.warning("Compiler metadata not found at %s", METADATA_FILE)
        return empty_metadata

    try:
        metadata = json.loads(
            METADATA_FILE.read_text(
                encoding="utf-8",
                errors="replace"
            )
        )

        if not isinstance(metadata, dict):
            logger.warning("Invalid compiler metadata format.")
            return empty_metadata

        return {
            "tokens": metadata.get("tokens", []),
            "ast": metadata.get(
                "ast",
                {
                    "type": "Program",
                    "children": []
                }
            )
        }

    except (OSError, json.JSONDecodeError) as exc:
        logger.warning("Failed to read compiler metadata: %s", exc)
        return empty_metadata




def read_assembly() -> Optional[str]:
    """
    Read the generated out.asm file, if it exists.

    Returns:
        The assembly source as a string, or None if the file is missing
        or unreadable.
    """
    if not ASSEMBLY_FILE.exists():
        logger.warning("Assembly file not found at %s", ASSEMBLY_FILE)
        return None

    try:
        return ASSEMBLY_FILE.read_text(encoding="utf-8", errors="replace")
    except OSError:
        logger.exception("Failed to read assembly file at %s", ASSEMBLY_FILE)
        return None


def copy_assembly_to_outputs() -> Optional[Path]:
    """
    Copy the freshly generated out.asm into backend/outputs/out.asm so the
    frontend (or a human reviewer) has a stable, persisted copy independent
    of the WSL build directory.

    Returns:
        The destination Path if the copy succeeded, otherwise None.
    """
    if not ASSEMBLY_FILE.exists():
        return None

    try:
        ensure_directories()
        shutil.copy2(ASSEMBLY_FILE, ASSEMBLY_OUTPUT_COPY)
        logger.info("Copied %s -> %s", ASSEMBLY_FILE, ASSEMBLY_OUTPUT_COPY)
        return ASSEMBLY_OUTPUT_COPY
    except OSError:
        logger.exception("Failed to copy assembly file to outputs directory.")
        return None


def cleanup() -> None:
    """
    Remove stale build artifacts (out.asm, out.o, out) and the previous
    source file before starting a new compile.

    This guarantees that a successful /compile response always reflects the
    *current* request rather than a leftover artifact from a previous run
    (e.g. if the new compilation fails before producing out.asm, we don't
    want to accidentally serve an old assembly file as if it were fresh).
    """
    stale_files = [
    SOURCE_FILE,
    METADATA_FILE,
    ASSEMBLY_FILE,
    OBJECT_FILE,
    EXECUTABLE_FILE
]
    for stale_file in stale_files:
        try:
            if stale_file.exists():
                stale_file.unlink()
                logger.debug("Removed stale file: %s", stale_file)
        except OSError:
            logger.exception("Failed to remove stale file: %s", stale_file)


def parse_compiler_errors(stderr: str) -> List[str]:
    """
    Split raw compiler stderr output into a clean list of individual error
    lines for easier consumption by the frontend (e.g. rendering each error
    as a separate item in the Monaco Editor's problems panel).

    Args:
        stderr: Raw stderr text captured from the compiler process.

    Returns:
        A list of non-empty, whitespace-trimmed error lines.
    """
    if not stderr:
        return []
    return [line.strip() for line in stderr.splitlines() if line.strip()]


# =============================================================================
# SECTION 5: FLASK APP INITIALISATION
# =============================================================================
app = Flask(__name__)

# Restrict CORS to the Vite dev server origin used by the React frontend.
CORS(app, resources={r"/*": {"origins": CORS_ORIGIN}})

# Make sure required directories exist as soon as the app boots.
ensure_directories()


# =============================================================================
# SECTION 6: ROUTES
# =============================================================================

@app.route("/files", methods=["GET"])
def list_files() -> tuple:
    """Return all persistent .g files in the Gamma workspace."""

    ensure_directories()

    files = sorted(
        file.name
        for file in PROJECT_FILES_DIR.iterdir()
        if file.is_file() and file.suffix.lower() == ".g"
    )

    return jsonify({
        "success": True,
        "files": files,
    }), 200


@app.route("/files", methods=["POST"])
def create_file() -> tuple:
    """Create a new empty Gamma .g file."""

    payload = request.get_json(silent=True)

    if payload is None:
        return jsonify({
            "success": False,
            "error": "Request body must be JSON.",
        }), 400

    filename = payload.get("name")

    if not is_valid_gamma_filename(filename):
        return jsonify({
            "success": False,
            "error": "Only simple .g filenames are allowed.",
        }), 400

    file_path = get_project_file_path(filename)

    if file_path is None:
        return jsonify({
            "success": False,
            "error": "Invalid Gamma filename.",
        }), 400

    if file_path.exists():
        return jsonify({
            "success": False,
            "error": "A file with this name already exists.",
        }), 409

    try:
        ensure_directories()

        content = "Start\n\nEnd"

        file_path.write_text(
            content,
            encoding="utf-8",
        )

        return jsonify({
            "success": True,
            "file": filename,
            "content": content,
        }), 201

    except OSError as exc:
        logger.exception("Failed to create file.")

        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500


@app.route("/files/<path:filename>", methods=["GET"])
def read_file(filename: str) -> tuple:
    """Read a Gamma source file."""

    file_path = get_project_file_path(filename)

    if file_path is None:
        return jsonify({
            "success": False,
            "error": "Only .g files are allowed.",
        }), 400

    if not file_path.exists() or not file_path.is_file():
        return jsonify({
            "success": False,
            "error": "File not found.",
        }), 404

    try:
        content = file_path.read_text(encoding="utf-8")

        return jsonify({
            "success": True,
            "file": filename,
            "content": content,
        }), 200

    except OSError as exc:
        logger.exception("Failed to read file.")

        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500


@app.route("/files/<path:filename>", methods=["PUT"])
def update_file(filename: str) -> tuple:
    """Update an existing Gamma source file."""

    file_path = get_project_file_path(filename)

    if file_path is None:
        return jsonify({
            "success": False,
            "error": "Only .g files are allowed.",
        }), 400

    if not file_path.exists():
        return jsonify({
            "success": False,
            "error": "File not found.",
        }), 404

    payload = request.get_json(silent=True)

    if payload is None or "content" not in payload:
        return jsonify({
            "success": False,
            "error": "Request must contain 'content'.",
        }), 400

    content = payload.get("content")

    if not isinstance(content, str):
        return jsonify({
            "success": False,
            "error": "File content must be a string.",
        }), 400

    try:
        file_path.write_text(
            content,
            encoding="utf-8",
        )

        return jsonify({
            "success": True,
            "file": filename,
        }), 200

    except OSError as exc:
        logger.exception("Failed to update file.")

        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500

@app.route("/health", methods=["GET"])
def health() -> tuple:
    """Simple liveness probe used by the frontend to check backend availability."""
    return jsonify({"status": "running"}), 200


@app.route("/compile", methods=["POST"])
def compile_endpoint() -> tuple:
    """
    POST /compile
    -------------
    Request body:
        { "code": "let x = 10; exit(x);" }

    Response body (200 or compile-failure 200 with success=false):
        {
            "success": bool,
            "compileTime": "0.482s",
            "stdout": "...",
            "stderr": "...",
            "assembly": "..." | null,
            "errors": ["..."]
        }
    """
    # ---- 1. Validate the incoming request -------------------------------
    payload = request.get_json(silent=True)
    if payload is None or "code" not in payload:
        logger.warning("Rejected /compile request: missing 'code' field.")
        return jsonify({
            "success": False,
            "error": "Request body must be JSON containing a 'code' field.",
        }), 400

    code = payload.get("code")
    if not isinstance(code, str) or not code.strip():
        logger.warning("Rejected /compile request: 'code' is empty or not a string.")
        return jsonify({
            "success": False,
            "error": "'code' must be a non-empty string.",
        }), 400

    # ---- 2. Clean workspace & persist the new source file ----------------
    try:
        cleanup()
        save_source(code)
    except OSError as exc:
        logger.exception("Failed to save source file.")
        return jsonify({
            "success": False,
            "error": f"Failed to save source file: {exc}",
        }), 500

    # ---- 3. Run the compiler ----------------------------------------------
    result = run_compiler()

    if result.error_message and result.timed_out:
        return jsonify({
            "success": False,
            "compileTime": f"{result.execution_time:.3f}s",
            "stdout": result.stdout,
            "stderr": result.stderr,
            "assembly": None,
            "errors": ["Compilation timed out."],
        }), 504

    if result.error_message:
        # Infrastructure-level failure (e.g. WSL missing), not a code error.
        return jsonify({
            "success": False,
            "compileTime": f"{result.execution_time:.3f}s",
            "stdout": result.stdout,
            "stderr": result.stderr,
            "assembly": None,
            "errors": [result.error_message],
        }), 500

    # ---- 4. Interpret the compiler's outcome -------------------------------
    assembly_content = read_assembly()
    success = (result.return_code == 0) and (assembly_content is not None)

    errors = parse_compiler_errors(result.stderr) if not success else []

    if success:
        copy_assembly_to_outputs()
        logger.info("Compilation succeeded in %.3fs", result.execution_time)
    else:
        logger.info(
            "Compilation failed (return_code=%s) in %.3fs",
            result.return_code,
            result.execution_time,
        )

    outcome = CompileOutcome(
        success=success,
        compile_time=result.execution_time,
        stdout=result.stdout,
        stderr=result.stderr,
        assembly=assembly_content,
        errors=errors,
    )

    metadata = read_compile_metadata()

    response_body = {
    "success": outcome.success,
    "compileTime": f"{outcome.compile_time:.3f}s",
    "stdout": outcome.stdout,
    "stderr": outcome.stderr,
    "assembly": outcome.assembly,
    "errors": outcome.errors,

    # Actual compiler metadata
    "tokens": metadata["tokens"],
    "ast": metadata["ast"],

    "output": outcome.stdout
}

    # 200 is returned even for a *compilation* failure (bad user source code)
    # because the *request itself* was handled successfully; the frontend
    # should inspect the "success" field to distinguish the two cases.
    return jsonify(response_body), 200


@app.route("/run", methods=["POST"])
def run_endpoint() -> tuple:
    """
    POST /run

    Starts the compiled Gamma program without blocking.

    Returns a run ID that the frontend uses to read output
    and send input to the running program.
    """
    if not EXECUTABLE_FILE.exists():
        logger.warning("Rejected /run request: no compiled executable found.")

        return jsonify({
            "success": False,
            "error": "No compiled executable found. Please compile first.",
        }), 400

    try:
        run_id = start_program()

        return jsonify({
            "success": True,
            "runId": run_id,
        }), 200

    except Exception as exc:
        logger.exception("Failed to start Gamma program.")

        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500

@app.route("/run/input", methods=["POST"])
def run_input_endpoint() -> tuple:
    """
    POST /run/input

    Sends user input to a running Gamma program.
    """
    payload = request.get_json(silent=True) or {}

    run_id = payload.get("runId")
    user_input = payload.get("input", "")

    if not isinstance(run_id, str) or not run_id:
        return jsonify({
            "success": False,
            "error": "Missing runId.",
        }), 400

    if not isinstance(user_input, str):
        user_input = str(user_input)

    with RUN_SESSIONS_LOCK:
        session = RUN_SESSIONS.get(run_id)

    if not session:
        return jsonify({
            "success": False,
            "error": "Run session not found.",
        }), 404

    process = session["process"]

    if process.poll() is not None:
        return jsonify({
            "success": False,
            "error": "Program has already finished.",
        }), 400

    fifo_path = session.get("fifo_path")
    if not fifo_path:
        return jsonify({
            "success": False,
            "error": "Input channel is unavailable.",
        }), 500

    try:
        write_command = (
            f"printf '%s\\n' {shlex.quote(user_input)} > "
            f"{shlex.quote(fifo_path)}"
        )

        write_result = subprocess.run(
            [
                "wsl.exe",
                "bash",
                "-lc",
                write_command,
            ],
            capture_output=True,
            text=True,
            timeout=5,
        )

        if write_result.returncode != 0:
            logger.error(
                "Failed to write program input: %s",
                write_result.stderr,
            )

            return jsonify({
                "success": False,
                "error": "Failed to send input to the running program.",
            }), 500

        return jsonify({
            "success": True,
        }), 200

    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Timed out while sending program input.",
        }), 504

    except Exception as exc:
        logger.exception("Failed to send program input.")

        return jsonify({
            "success": False,
            "error": str(exc),
        }), 500

@app.route("/run/status", methods=["GET"])
def run_status_endpoint() -> tuple:
    """
    GET /run/status?runId=...

    Returns the current output and state of a running Gamma program.
    """
    run_id = request.args.get("runId", "")

    if not run_id:
        return jsonify({
            "success": False,
            "error": "Missing runId.",
        }), 400

    with RUN_SESSIONS_LOCK:
        session = RUN_SESSIONS.get(run_id)

        if not session:
            return jsonify({
                "success": False,
                "error": "Run session not found.",
            }), 404

        return jsonify({
            "success": True,
            "output": session["output"],
            "finished": session["finished"],
            "exitCode": session["exit_code"],
        }), 200

@app.errorhandler(404)
def not_found(_error) -> tuple:
    return jsonify({"success": False, "error": "Endpoint not found."}), 404


@app.errorhandler(405)
def method_not_allowed(_error) -> tuple:
    return jsonify({"success": False, "error": "HTTP method not allowed for this endpoint."}), 405


@app.errorhandler(500)
def internal_server_error(_error) -> tuple:
    logger.exception("Unhandled internal server error.")
    return jsonify({"success": False, "error": "Internal server error."}), 500


# =============================================================================
# SECTION 8: ENTRYPOINT
# =============================================================================
if __name__ == "__main__":
    logger.info("Starting Gamma Compiler backend on http://127.0.0.1:5000")
    logger.info("Project root: %s", PROJECT_ROOT)
    logger.info("Build dir:    %s", BUILD_DIR)
    app.run(host="127.0.0.1", port=5000, debug=True)
