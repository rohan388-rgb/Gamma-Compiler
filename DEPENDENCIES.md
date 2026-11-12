# Gamma Compiler – Dependencies and Requirements

## 1. Compiler Dependencies

| Dependency | Purpose |
|---|---|
| C++ Compiler | Builds the Gamma compiler source code |
| CMake | Configures and manages the compiler build |
| NASM | Assembles generated x86-64 assembly code |
| Linux / WSL | Provides the compiler build and execution environment |
| GNU Linker (ld) | Links object files into an executable |

## 2. Backend Dependencies

The backend uses Python and Flask.

Dependency file: `backend/requirements.txt`

Install the dependencies:

```bash
cd backend
pip install -r requirements.txt
```

## 3. Frontend Dependencies

The frontend uses React and TypeScript.

Dependency files:
- `frontend/package.json`
- `frontend/package-lock.json`

Install the dependencies:

```bash
cd frontend
npm install
```

## 4. Development Tools

| Tool | Purpose |
|---|---|
| Git | Version control |
| GitHub | Remote repository hosting |
| VS Code / CLion | Code development |
| WSL | Linux development environment |

## 5. Dependency Management

- Python packages are managed using pip.
- JavaScript packages are managed using npm.
- C++ build configuration is managed using CMake.
- Assembly generation uses NASM.
- Executable linking uses the GNU linker.

For exact package versions, refer to the backend requirements file and frontend package-lock file.