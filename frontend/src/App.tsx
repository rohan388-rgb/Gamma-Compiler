import {useState,useCallback,useRef,useEffect,type ReactNode,} from "react";
import Editor, { type OnMount } from "@monaco-editor/react";
import api from "./services/api";
import {
  VscFiles,
  VscChevronRight,
  VscChevronDown,
  VscFile,
  VscFolder,
  VscFolderOpened,
  VscSettingsGear,
  VscGithubInverted,
  VscPlay,
  VscClearAll,
} from "react-icons/vsc";
import {
  FiZap,
  FiClock,
  FiCode,
  FiAlertTriangle,
  FiXCircle,
  FiLayers,
  FiHash,
  FiCheckCircle,
  FiPlus,
  FiX,
} from "react-icons/fi";
import { RiCpuLine } from "react-icons/ri";

// ─── Types ──────────────────────────────────────────────────────────────────

interface Token {
  type: string;
  value: string;
  line: number;
}

interface ASTNode {
  type: string;
  value?: string;
  children?: ASTNode[];
}

interface CompileError {
  line: number;
  message: string;
  source: string;
  pointer: string;
}

interface CompileResult {
  output: string;
  tokens: Token[];
  ast: ASTNode;
  assembly: string;
  errors: CompileError[];
}

type TabId = "output" | "tokens" | "ast" | "assembly" | "errors";

// ─── Example Programs ──────────────────────────────────────────────────────

// ─── Example Programs ──────────────────────────────────────────────────────

const EXAMPLES: Record<string, string> = {
  "1. Hello World": `Start

Show "Hello World".

End`,

  "2. Input": `Start

Let text name to "".
Let number age to 0.

Ask "Enter your name" and store in name.
Ask "Enter your age" and store in age.

Show name.
Show age.

End`,

  "3. Arithmetic": `Start

Let number a to 0.
Let number b to 0.

Ask "Enter first number" and store in a.
Ask "Enter second number" and store in b.

Let number sum to a plus b.
Let number difference to a minus b.
Let number product to a times b.
Let number quotient to a divided by b.
Let number rem to a remainder b.

Show sum.
Show difference.
Show product.
Show quotient.
Show rem.

End`,

  "4. Switch Calculator": `Start

Let number choice to 0.
Let number a to 0.
Let number b to 0.

Ask "Enter 1 for addition, 2 for subtraction, 3 for multiplication, 4 for division" and store in choice.
Ask "Enter first number" and store in a.
Ask "Enter second number" and store in b.

Choose (choice) {
    Option 1: {
        Show a plus b.
    }

    Option 2: {
        Show a minus b.
    }

    Option 3: {
        Show a times b.
    }

    Option 4: {
        Show a divided by b.
    }

    Other: {
        Show "Invalid choice".
    }
}

End`,

  "5. If Else Otherwise": `Start

Let number marks to 0.

Ask "Enter your marks" and store in marks.

If marks is greater than 90 {
    Show "Excellent".
} Else if marks is greater than 60 {
    Show "Good".
} Otherwise {
    Show "Needs improvement".
}

End`,

  "6. For Loop": `Start

Let number first to 0.
Let number last to 0.

Ask "Enter starting number" and store in first.
Ask "Enter ending number" and store in last.

For i from first to last {
    Show i.
}

End`,

  "7. While Loop": `Start

Let number current to 0.
Let number limit to 0.

Ask "Enter starting number" and store in current.
Ask "Enter limit" and store in limit.

While current is less than or equal to limit {
    Show current.
    Set current to current plus 1.
}

End`,

  "8. Do While": `Start

Let number current to 0.
Let number limit to 0.

Ask "Enter starting number" and store in current.
Ask "Enter limit" and store in limit.

Do {
    Show current.
    Set current to current plus 1.
} While current is less than or equal to limit.

End`,

  "9. Next": `Start

Let number first to 0.
Let number last to 0.

Ask "Enter starting number" and store in first.
Ask "Enter ending number" and store in last.

For i from first to last {
    If i is equal to 3 {
        next.
    }

    Show i.
}

End`,

  "10. Functions": `Start

Define function square with number x {
    Give x times x.
}

Let number n to 0.

Ask "Enter a number" and store in n.

Show Call square with n.

End`,

  "11. Array Index and Replace": `Start

Let array numbers to [10, 20, 30, 40, 50].
Let number pos to 0.
Let number newValue to 0.

Ask "Enter index" and store in pos.
Ask "Enter new value" and store in newValue.

Show numbers[pos].

Set numbers[pos] to newValue.

Show numbers[pos].

End`,

  "12. String": `Start

Let text sentence to "".

Ask "Enter a string" and store in sentence.

Show sentence.

End`,

  "13. Structure": `Start

Struct Person {
    name.
    age.
}

End`,

  "14. Enum": `Start

Enum Color {
    Red,
    Green,
    Blue
}

Show Green.

End`,

  "15. Even or Odd Number": `Start

Let number n to 0.

Ask "Enter a number" and store in n.

If n remainder 2 is equal to 0 {
    Show "Even".
} Otherwise {
    Show "Odd".
}

End`,

  "16. Positive Negative or Zero": `Start

Let number n to 0.

Ask "Enter a number" and store in n.

If n is greater than 0 {
    Show "Positive".
} Else if n is less than 0 {
    Show "Negative".
} Otherwise {
    Show "Zero".
}

End`,

  "17. Largest of Two Numbers": `Start

Let number a to 0.
Let number b to 0.

Ask "Enter first number" and store in a.
Ask "Enter second number" and store in b.

If a is greater than b {
    Show a.
} Otherwise {
    Show b.
}

End`,

  "18. Largest of Three Numbers": `Start

Let number a to 0.
Let number b to 0.
Let number c to 0.

Ask "Enter first number" and store in a.
Ask "Enter second number" and store in b.
Ask "Enter third number" and store in c.

If a is greater than b {
    If a is greater than c {
        Show a.
    } Otherwise {
        Show c.
    }
} Otherwise {
    If b is greater than c {
        Show b.
    } Otherwise {
        Show c.
    }
}

End`,

  "19. Factorial": `Start

Let number n to 0.
Let number fact to 1.
Let number i to 1.

Ask "Enter a non-negative number" and store in n.

While i is less than or equal to n {
    Set fact to fact times i.
    Set i to i plus 1.
}

Show fact.

End`,

  "20. Palindrome Number": `Start

Let number n to 0.
Let number original to 0.
Let number reverse to 0.
Let number digit to 0.

Ask "Enter a number" and store in n.

Set original to n.

While n is greater than 0 {
    Set digit to n remainder 10.
    Set reverse to reverse times 10 plus digit.
    Set n to n divided by 10.
}

If reverse is equal to original {
    Show "Palindrome".
} Otherwise {
    Show "Not Palindrome".
}

End`,

  "21. Prime Number": `Start

Let number n to 0.
Let number i to 2.
Let boolean prime to true.

Ask "Enter a number" and store in n.

If n is less than 2 {
    Set prime to false.
}

While i times i is less than or equal to n {
    If n remainder i is equal to 0 {
        Set prime to false.
        Stop.
    }
    Set i to i plus 1.
}

If prime {
    Show "Prime".
} Otherwise {
    Show "Not Prime".
}

End`,

  "22. Reverse a Number": `Start

Let number n to 0.
Let number reverse to 0.
Let number digit to 0.

Ask "Enter a non-negative number" and store in n.

While n is greater than 0 {
    Set digit to n remainder 10.
    Set reverse to reverse times 10 plus digit.
    Set n to n divided by 10.
}

Show reverse.

End`,

  "23. Fibonacci Series": `Start

Let number n to 0.
Let number a to 0.
Let number b to 1.
Let number next to 0.
Let number i to 0.

Ask "Enter number of terms" and store in n.

While i is less than n {
    Show a.
    Set next to a plus b.
    Set a to b.
    Set b to next.
    Set i to i plus 1.
}

End`,

  "24. Sum of Digits": `Start

Let number n to 0.
Let number digit to 0.
Let number sum to 0.

Ask "Enter a non-negative number" and store in n.

While n is greater than 0 {
    Set digit to n remainder 10.
    Set sum to sum plus digit.
    Set n to n divided by 10.
}

Show sum.

End`,

  "25. Armstrong Number": `Start

Let number n to 0.
Let number original to 0.
Let number temp to 0.
Let number digit to 0.
Let number sum to 0.

Ask "Enter a three-digit number" and store in n.

Set original to n.
Set temp to n.

While temp is greater than 0 {
    Set digit to temp remainder 10.
    Set sum to sum plus digit times digit times digit.
    Set temp to temp divided by 10.
}

If sum is equal to original {
    Show "Armstrong".
} Otherwise {
    Show "Not Armstrong".
}

End`,

  "26. Perfect Number": `Start

Let number n to 0.
Let number i to 1.
Let number sum to 0.

Ask "Enter a positive number" and store in n.

While i is less than n {
    If n remainder i is equal to 0 {
        Set sum to sum plus i.
    }
    Set i to i plus 1.
}

If sum is equal to n {
    Show "Perfect Number".
} Otherwise {
    Show "Not a Perfect Number".
}

End`,

  "27. Strong Number": `Start

Let number n to 0.
Let number temp to 0.
Let number digit to 0.
Let number fact to 1.
Let number sum to 0.
Let number i to 1.

Ask "Enter a non-negative number" and store in n.

Set temp to n.

While temp is greater than 0 {
    Set digit to temp remainder 10.
    Set fact to 1.
    Set i to 1.

    While i is less than or equal to digit {
        Set fact to fact times i.
        Set i to i plus 1.
    }

    Set sum to sum plus fact.
    Set temp to temp divided by 10.
}

If sum is equal to n {
    Show "Strong Number".
} Otherwise {
    Show "Not a Strong Number".
}

End`,

  "28. Automorphic Number": `Start

Let number n to 0.
Let number square to 0.
Let number temp to 0.
Let number divisor to 1.

Ask "Enter a non-negative number" and store in n.

Set square to n times n.
Set temp to n.

While temp is greater than 0 {
    Set divisor to divisor times 10.
    Set temp to temp divided by 10.
}

If square remainder divisor is equal to n {
    Show "Automorphic Number".
} Otherwise {
    Show "Not an Automorphic Number".
}

End`,

  "29. Neon Number": `Start

Let number n to 0.
Let number square to 0.
Let number digit to 0.
Let number sum to 0.

Ask "Enter a non-negative number" and store in n.

Set square to n times n.

While square is greater than 0 {
    Set digit to square remainder 10.
    Set sum to sum plus digit.
    Set square to square divided by 10.
}

If sum is equal to n {
    Show "Neon Number".
} Otherwise {
    Show "Not a Neon Number".
}

End`,

  "30. GCD of Two Numbers": `Start

Let number a to 0.
Let number b to 0.
Let number rem to 0.

Ask "Enter first number" and store in a.
Ask "Enter second number" and store in b.

While b is not equal to 0 {
    Set rem to a remainder b.
    Set a to b.
    Set b to rem.
}

Show a.

End`,

  "31. LCM of Two Numbers": `Start

Let number a to 0.
Let number b to 0.
Let number x to 0.
Let number y to 0.
Let number rem to 0.
Let number gcd to 0.
Let number lcm to 0.

Ask "Enter first positive number" and store in a.
Ask "Enter second positive number" and store in b.

Set x to a.
Set y to b.

While y is not equal to 0 {
    Set rem to x remainder y.
    Set x to y.
    Set y to rem.
}

Set gcd to x.
Set lcm to a times b divided by gcd.

Show lcm.

End`,

  "32. Multiplication Table": `Start

Let number n to 0.
Let number i to 1.

Ask "Enter a number" and store in n.

While i is less than or equal to 10 {
    Show n times i.
    Set i to i plus 1.
}

End`,

  "33. Count Digits": `Start

Let number n to 0.
Let number count to 0.

Ask "Enter a non-negative number" and store in n.

If n is equal to 0 {
    Set count to 1.
} Otherwise {
    While n is greater than 0 {
        Set count to count plus 1.
        Set n to n divided by 10.
    }
}

Show count.

End`,

  "34. Smallest Digit": `Start

Let number n to 0.
Let number digit to 0.
Let number smallest to 9.

Ask "Enter a non-negative number" and store in n.

If n is equal to 0 {
    Set smallest to 0.
}

While n is greater than 0 {
    Set digit to n remainder 10.
    If digit is less than smallest {
        Set smallest to digit.
    }
    Set n to n divided by 10.
}

Show smallest.

End`,

  "35. Largest Digit": `Start

Let number n to 0.
Let number digit to 0.
Let number largest to 0.

Ask "Enter a non-negative number" and store in n.

While n is greater than 0 {
    Set digit to n remainder 10.
    If digit is greater than largest {
        Set largest to digit.
    }
    Set n to n divided by 10.
}

Show largest.

End`,

  "36. Swap Two Numbers": `Start

Let number a to 0.
Let number b to 0.
Let number temp to 0.

Ask "Enter first number" and store in a.
Ask "Enter second number" and store in b.

Set temp to a.
Set a to b.
Set b to temp.

Show a.
Show b.

End`,

};

const INITIAL_CODE = `Start

Show "Hello World".

End`;


// ─── Monaco Language Setup ─────────────────────────────────────────────────

// eslint-disable-next-line @typescript-eslint/no-explicit-any
function registerGammaLanguage(monacoInstance: any) {
  monacoInstance.languages.register({ id: "gamma" });

  monacoInstance.languages.setMonarchTokensProvider("gamma", {
    keywords: [
  // Program
  "Start",
  "End",

  // Variables and output
  "Let",
  "Show",
  "Set",

  // Conditions
  "If",
  "Otherwise",
  "Else",
  "if",

  // Loops
  "While",
  "For",

  // Functions
  "Call",
  "Define",
  "Give",
  "Stop",
  "function",
  "parameter",
  "with",

  // Loop control
  "next",
  "continue",

  // Types
  "number",
  "decimal",
  "text",
  "boolean",

  // Boolean values
  "true",
  "false",

  // Comparison words
  "is",
  "greater",
  "less",
  "than",
  "or",
  "equal",
  "not",

  // Arithmetic words
  "plus",
  "minus",
  "times",
  "divided",
  "remainder",
],
    tokenizer: {
      root: [
        [/\/\/.*$/, "comment"],
        [/"([^"\\]|\\.)*"/, "string"],
        [/\b\d+(\.\d+)?\b/, "number"],
        [
          /[a-zA-Z_]\w*/,
          {
            cases: {
              "@keywords": "keyword",
              "@default": "identifier",
            },
          },
        ],
        [/[{}()\[\]]/, "delimiter.bracket"],
        [/->/, "operator"],
        [/[+\-*/%=<>!&|^~]+/, "operator"],
        [/[;,.]/, "delimiter"],
      ],
    },
  });

  monacoInstance.editor.defineTheme("gamma-dark", {
    base: "vs-dark",
    inherit: true,
    rules: [
      { token: "keyword", foreground: "C792EA", fontStyle: "bold" },
      { token: "string", foreground: "C3E88D" },
      { token: "number", foreground: "F78C6C" },
      { token: "comment", foreground: "546E7A", fontStyle: "italic" },
      { token: "operator", foreground: "89DDFF" },
      { token: "delimiter.bracket", foreground: "FFCB6B" },
      { token: "identifier", foreground: "CDD3DE" },
      { token: "delimiter", foreground: "89DDFF" },
    ],
    colors: {
      "editor.background": "#1E1E1E",
      "editor.foreground": "#CDD3DE",
      "editorLineNumber.foreground": "#3B4252",
      "editorLineNumber.activeForeground": "#7B8CAA",
      "editor.selectionBackground": "#2D4A6A80",
      "editor.lineHighlightBackground": "#252525",
      "editorCursor.foreground": "#528BFF",
      "editor.inactiveSelectionBackground": "#1E273340",
      "editorBracketMatch.background": "#3B4A6B40",
      "editorBracketMatch.border": "#528BFF80",
      "editorIndentGuide.background1": "#2A2A2A",
      "editorIndentGuide.activeBackground1": "#3B4252",
      "scrollbar.shadow": "#0D1117",
      "scrollbarSlider.background": "#30363D60",
      "scrollbarSlider.hoverBackground": "#30363D90",
      "scrollbarSlider.activeBackground": "#30363D",
      "minimap.background": "#1A1A1A",
    },
  });

  monacoInstance.editor.setTheme("gamma-dark");
}

// ─── Utility: Count AST Nodes ───────────────────────────────────────────────

function countNodes(node: ASTNode | null | undefined): number {
  if (!node) return 0;
  return 1 + (node.children?.reduce((s, c) => s + countNodes(c), 0) ?? 0);
}

// ─── Component: GammaLogo ──────────────────────────────────────────────────

function GammaLogo() {
  return (
    <div
      className="w-7 h-7 rounded-md flex items-center justify-center text-white font-bold text-base shrink-0 select-none"
      style={{ background: "linear-gradient(135deg, #1F6FEB 0%, #A371F7 100%)" }}
    >
      γ
    </div>
  );
}

// ─── Component: ASTTreeNode ────────────────────────────────────────────────

function ASTTreeNode({ node, depth = 0 }: { node: ASTNode; depth?: number }) {
  const [expanded, setExpanded] = useState(true);
  const hasChildren = (node.children?.length ?? 0) > 0;

  return (
    <div className="font-mono text-xs leading-relaxed select-none">
      <div
        className="flex items-center gap-1 py-0.5 rounded hover:bg-white/5 cursor-pointer transition-colors"
        style={{ paddingLeft: `${depth * 18 + 8}px`, paddingRight: 8 }}
        onClick={() => hasChildren && setExpanded((p) => !p)}
      >
        <span className="w-3 shrink-0 flex items-center justify-center">
          {hasChildren ? (
            expanded ? (
              <VscChevronDown size={11} className="text-gray-500" />
            ) : (
              <VscChevronRight size={11} className="text-gray-500" />
            )
          ) : null}
        </span>
        <span className="text-purple-400 font-medium">{node.type}</span>
        {node.value && (
          <span className="text-orange-300 ml-0.5">
            <span className="text-gray-500">(</span>
            {node.value}
            <span className="text-gray-500">)</span>
          </span>
        )}
      </div>
      {expanded && hasChildren && (
        <div>
          {node.children!.map((child, i) => (
            <ASTTreeNode key={i} node={child} depth={depth + 1} />
          ))}
        </div>
      )}
    </div>
  );
}

// ─── Component: AssemblyView ────────────────────────────────────────────────

function AssemblyView({ code }: { code: string }) {
  if (!code) {
    return (
      <div className="p-4 font-mono text-sm text-gray-500 italic">
        No assembly output. Compile first.
      </div>
    );
  }

  const lines = code.split("\n");

  return (
    <pre className="h-full overflow-auto p-4 font-mono text-[13px] leading-6">
      {lines.map((line, i) => {
        const trimmed = line.trimStart();
        const indent = line.slice(0, line.length - trimmed.length);

        if (!trimmed) return <div key={i}>&nbsp;</div>;

        if (/^(global|extern|section)/.test(trimmed)) {
          return (
            <div key={i}>
              <span className="text-blue-400">{line}</span>
            </div>
          );
        }

        if (trimmed.endsWith(":")) {
          return (
            <div key={i}>
              <span className="text-yellow-300">{line}</span>
            </div>
          );
        }

        const instrMatch = trimmed.match(
          /^(mov|syscall|push|pop|call|ret|lea|sub|add|xor|cmp|jmp|je|jne|jl|jg|int|nop|imul|idiv|neg|not|shl|shr|and|or)\b/
        );
        if (instrMatch) {
          const instr = instrMatch[1];
          const rest = trimmed.slice(instr.length);
          return (
            <div key={i}>
              <span className="text-gray-400">{indent}</span>
              <span className="text-cyan-400 font-medium">{instr}</span>
              <span className="text-gray-200">{rest}</span>
            </div>
          );
        }

        return (
          <div key={i}>
            <span className="text-gray-400">{line}</span>
          </div>
        );
      })}
    </pre>
  );
}

// ─── Component: StatCard ───────────────────────────────────────────────────

function StatCard({
  icon,
  label,
  value,
  colorClass,
}: {
  icon: ReactNode;
  label: string;
  value: string | number;
  colorClass: string;
}) {
  return (
    <div
      className="rounded-lg p-3 border border-white/5 flex flex-col gap-1"
      style={{ backgroundColor: "#21262D" }}
    >
      <div className="flex items-center gap-1.5">
        <span className={colorClass}>{icon}</span>
        <span className="text-[11px] text-gray-500 uppercase tracking-wide font-medium">
          {label}
        </span>
      </div>
      <div className="text-xl font-bold text-white font-mono leading-none">
        {value}
      </div>
    </div>
  );
}

// ─── Component: FileExplorer ───────────────────────────────────────────────

function FileExplorer({
  projectFiles,
  activeFile,
  onOpenFile,
  onNewFile,
  onLoadExample,
}: {
  projectFiles: string[];
  activeFile: string;
  onOpenFile: (fileName: string) => void;
  onNewFile: () => void;
  onLoadExample: (code: string) => void;
}) {
  const [explorerOpen, setExplorerOpen] = useState(true);
  const [examplesOpen, setExamplesOpen] = useState(true);

  return (
    <div
      className="flex flex-col h-full overflow-hidden"
      style={{ backgroundColor: "#161B22" }}
    >
      {/* Explorer Header */}
      <div className="flex items-center justify-between px-3 py-2 border-b border-white/5 shrink-0">
        <span className="text-[11px] font-semibold text-gray-400 uppercase tracking-widest">
          Explorer
        </span>

        {/* + New File */}
        <button
          onClick={onNewFile}
          className="p-1 rounded hover:bg-white/10 transition-colors text-gray-400 hover:text-white"
          title="New Gamma File"
        >
          <FiPlus size={15} />
        </button>
      </div>

      <div className="flex-1 overflow-y-auto py-1">

        {/* Project */}
        <button
          className="w-full flex items-center gap-1 px-2 py-1 text-xs hover:bg-white/5 transition-colors text-gray-300"
          onClick={() => setExplorerOpen((p) => !p)}
        >
          {explorerOpen ? (
            <VscChevronDown
              size={13}
              className="text-gray-500 shrink-0"
            />
          ) : (
            <VscChevronRight
              size={13}
              className="text-gray-500 shrink-0"
            />
          )}

          {explorerOpen ? (
            <VscFolderOpened
              size={14}
              className="text-yellow-400 shrink-0"
            />
          ) : (
            <VscFolder
              size={14}
              className="text-yellow-400 shrink-0"
            />
          )}

          <span className="font-semibold ml-0.5">
            Project
          </span>
        </button>

        {explorerOpen && (
          <div>
            {projectFiles.map((fileName) => (
              <button
                key={fileName}
                onClick={() => onOpenFile(fileName)}
                className={`w-full flex items-center gap-2 py-1 text-xs cursor-pointer transition-colors ${
                  fileName === activeFile
                    ? "text-white"
                    : "text-gray-500 hover:text-gray-200 hover:bg-white/5"
                }`}
                style={{
                  paddingLeft: 32,
                  paddingRight: 12,
                  backgroundColor:
                    fileName === activeFile
                      ? "#2D333B"
                      : undefined,
                }}
              >
                <VscFile
                  size={13}
                  className={
                    fileName === activeFile
                      ? "text-blue-400 shrink-0"
                      : "text-gray-600 shrink-0"
                  }
                />

                <span className="truncate">
                  {fileName}
                </span>
              </button>
            ))}
          </div>
        )}

        {/* Examples */}
        <div className="mt-2">
          <button
            className="w-full flex items-center gap-1 px-2 py-1 text-xs hover:bg-white/5 transition-colors text-gray-300"
            onClick={() => setExamplesOpen((p) => !p)}
          >
            {examplesOpen ? (
              <VscChevronDown
                size={13}
                className="text-gray-500 shrink-0"
              />
            ) : (
              <VscChevronRight
                size={13}
                className="text-gray-500 shrink-0"
              />
            )}

            {examplesOpen ? (
              <VscFolderOpened
                size={14}
                className="text-purple-400 shrink-0"
              />
            ) : (
              <VscFolder
                size={14}
                className="text-purple-400 shrink-0"
              />
            )}

            <span className="font-semibold ml-0.5">
              Examples
            </span>
          </button>

          {examplesOpen && (
            <div>
              {Object.entries(EXAMPLES).map(
                ([name, exampleCode]) => (
                  <button
                    key={name}
                    onClick={() => onLoadExample(exampleCode)}
                    className="w-full flex items-center gap-2 py-1 text-xs text-gray-500 hover:text-gray-200 hover:bg-white/5 transition-colors text-left"
                    style={{
                      paddingLeft: 32,
                      paddingRight: 12,
                    }}
                  >
                    <VscFile
                      size={13}
                      className="text-gray-600 shrink-0"
                    />

                    <span className="truncate">
                      {name}.g
                    </span>
                  </button>
                )
              )}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

// ─── Component: OutputPanel ────────────────────────────────────────────────

function OutputPanel({
  result,
  activeTab,
  onTabChange,
  compileStatus,
  programInput,
  setProgramInput,
  activeRunId,
}: {
  result: CompileResult;
  activeTab: TabId;
  onTabChange: (tab: TabId) => void;
  compileStatus: "idle" | "success" | "error";
  programInput: string;
  setProgramInput: (value: string) => void;
  activeRunId: string | null;
}) {
  const programInputRef = useRef<HTMLInputElement>(null);

useEffect(() => {
  if (!activeRunId) {
    return;
  }

  const frame = window.requestAnimationFrame(() => {
    programInputRef.current?.focus();
  });

  return () => {
    window.cancelAnimationFrame(frame);
  };
}, [activeRunId]);
  const tabs: { id: TabId; label: string; badge?: number }[] = [
    { id: "output", label: "Output" },
    { id: "tokens", label: "Tokens" },
    { id: "ast", label: "AST" },
    { id: "assembly", label: "Assembly" },
    { id: "errors", label: "Errors", badge: result.errors?.length ?? 0 },
  ];

  return (
    <div className="flex flex-col h-full overflow-hidden">
      {/* Tab bar */}
      <div
        className="flex items-center border-b border-white/10 shrink-0 overflow-x-auto"
        style={{ backgroundColor: "#0D1117" }}
      >
        {tabs.map((tab) => (
          <button
            key={tab.id}
            onClick={() => onTabChange(tab.id)}
            className={`flex items-center gap-1.5 px-4 py-2 text-xs font-medium transition-colors whitespace-nowrap border-b-2 ${
              activeTab === tab.id
                ? "border-blue-500 text-white"
                : "border-transparent text-gray-500 hover:text-gray-300 hover:bg-white/5"
            }`}
          >
            {tab.label}
            {tab.badge !== undefined && tab.badge > 0 && (
              <span className="bg-red-500 text-white text-[10px] rounded-full min-w-[16px] h-4 flex items-center justify-center px-1 leading-none">
                {tab.badge}
              </span>
            )}
          </button>
        ))}
      </div>

      {/* Content */}
      <div className="flex-1 overflow-hidden" style={{ backgroundColor: "#161B22" }}>
        {/* Output */}
        {activeTab === "output" && (
  <div
    className="h-full flex flex-col font-mono text-[13px]"
    style={{ backgroundColor: "#161B22" }}
  >
    {/* Terminal output */}
    <div className="flex-1 overflow-y-auto p-4 leading-6">
      {result.output ? (
        result.output.split("\n").map((line, i) => (
          <div
            key={i}
            className={
              i === 0 && compileStatus === "success"
                ? "text-green-400 font-semibold"
                : i === 0 && compileStatus === "error"
                ? "text-red-400 font-semibold"
                : "text-gray-300"
            }
          >
            {line}
          </div>
        ))
      ) : (
        <span className="text-gray-500 italic text-sm">
          No output yet. Click Run to execute.
        </span>
      )}
    </div>

    {/* Terminal input */}
    <div
      className="shrink-0 border-t border-white/10 px-4 py-2 flex items-center gap-2"
      style={{ backgroundColor: "#0D1117" }}
    >
      <span className="text-green-400 select-none">&gt;</span>

      <input
          ref={programInputRef}
          type="text"
          value={programInput}
          onChange={(e) => setProgramInput(e.target.value)}
          onKeyDown={async (e) => {
            if (e.key !== "Enter") {
              return;
            }

            e.preventDefault();

            const value = programInput.trim();

            if (!activeRunId || !value) {
              return;
            }

            console.log("SENDING INPUT:", value);
            console.log("RUN ID:", activeRunId);

            try {
              await api.post("/run/input", {
                runId: activeRunId,
                input: value,
              });

              setProgramInput("");

              window.requestAnimationFrame(() => {
                programInputRef.current?.focus();
              });
            } catch (error) {
              console.error("Failed to send program input:", error);
            }
          }}
          placeholder="Enter program input..."
          className="flex-1 bg-transparent border-none outline-none text-gray-200 text-[13px] font-mono placeholder:text-gray-600"
        />
    </div>
  </div>
)}

        {/* Tokens */}
        {activeTab === "tokens" && (
          <div className="h-full overflow-y-auto">
            {result.tokens.length > 0 ? (
              <table className="w-full text-xs font-mono border-separate border-spacing-0">
                <thead className="sticky top-0 z-10" style={{ backgroundColor: "#0D1117" }}>
                  <tr>
                    {["Type", "Value", "Line"].map((h) => (
                      <th
                        key={h}
                        className="text-left px-4 py-2.5 text-[11px] font-semibold text-gray-400 uppercase tracking-wider border-b border-white/10"
                      >
                        {h}
                      </th>
                    ))}
                  </tr>
                </thead>
                <tbody>
                  {result.tokens.map((tok, i) => (
                    <tr
                      key={i}
                      className="border-b border-white/5 hover:bg-white/5 transition-colors"
                    >
                      <td className="px-4 py-2 text-purple-400 font-medium">{tok.type}</td>
                      <td className="px-4 py-2 text-orange-300">{tok.value || <span className="text-gray-600">—</span>}</td>
                      <td className="px-4 py-2 text-gray-500">{tok.line}</td>
                    </tr>
                  ))}
                </tbody>
              </table> 
            ) : (
              <div className="p-4 text-gray-500 italic text-sm">
                No tokens. Compile first.
              </div>
            )}
          </div>
        )}

        {/* AST */}
        {activeTab === "ast" && (
          <div className="h-full overflow-y-auto p-2">
            {result.ast?.type ? (
              <ASTTreeNode node={result.ast} />
            ) : (
              <div className="p-4 text-gray-500 italic text-sm">
                No AST. Compile first.
              </div>
            )}
          </div>
        )}

        {/* Assembly */}
        {activeTab === "assembly" && (
          <div className="h-full overflow-hidden" style={{ backgroundColor: "#1E1E1E" }}>
            <AssemblyView code={result.assembly} />
          </div>
        )}

            {/* Errors */}
{activeTab === "errors" && (
  <div className="h-full overflow-y-auto p-4">
    {result.errors.length > 0 ? (
      <div className="flex flex-col gap-3">
        {result.errors.map((err, i) => (
          <div
            key={i}
            className="rounded-lg p-3 border border-red-500/20"
            style={{ backgroundColor: "#2A1515" }}
          >
            <div className="flex items-center gap-2">
              <FiXCircle
                size={13}
                className="text-red-400 shrink-0"
              />

              <span className="text-red-300 text-xs font-mono whitespace-pre-wrap">
                {typeof err === "string" ? err : err.message}
              </span>
            </div>
          </div>
        ))}
      </div>
    ) : (
      <div className="flex items-center gap-2 text-green-400 text-sm">
        <FiCheckCircle size={15} />
        <span>No errors detected.</span>
          </div>
        )}
      </div>
      )}
      </div>
    </div>
  );    
}

export default function App() {
  const [code, setCode] = useState(INITIAL_CODE);
    const [projectFiles, setProjectFiles] = useState<string[]>([
    "test.g",
  ]);

  const [activeFile, setActiveFile] = useState("test.g");

  const [openFiles, setOpenFiles] = useState<string[]>([
    "test.g",
  ]);

  const [fileContents, setFileContents] = useState<Record<string, string>>({
    "test.g": INITIAL_CODE,
  });
  const [activeTab, setActiveTab] = useState<TabId>("output");
  const [result, setResult] = useState<CompileResult>({
  output: "",
  tokens: [],
  ast: {
    type: "Program",
    children: [],
  },
  assembly: "",
  errors: [],
});
  const [isCompiling, setIsCompiling] = useState(false);
  const [isRunning, setIsRunning] = useState(false);
  const [programInput, setProgramInput] = useState("");
  const [activeRunId, setActiveRunId] = useState<string | null>(null);
  const [sidebarOpen, setSidebarOpen] = useState(true);
  const [compileTime, setCompileTime] = useState(0.042);
  const [compileStatus, setCompileStatus] = useState<"idle" | "success" | "error">("success");
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  const editorRef = useRef<any>(null);

  const handleEditorMount: OnMount = useCallback((editor, monacoInstance) => {
    editorRef.current = editor;
    registerGammaLanguage(monacoInstance);
  }, []);

    const handleNewFile = useCallback(async () => {
  const input = window.prompt(
    "Enter Gamma file name:",
    "newfile.g"
  );

  if (!input) {
    return;
  }

  const trimmed = input.trim();

  if (!trimmed) {
    return;
  }

  const fileName = trimmed.includes(".")
    ? trimmed
    : `${trimmed}.g`;

  // Only .g files are allowed
  if (!fileName.toLowerCase().endsWith(".g")) {
    window.alert("Only .g Gamma files are allowed.");
    return;
  }

  // Prevent duplicate files
  if (projectFiles.includes(fileName)) {
    window.alert("A file with this name already exists.");
    return;
  }

  try {
    const response = await api.post("/files", {
      name: fileName,
    });

    const createdFile = response.data.file;
    const content = response.data.content ?? "";

    setProjectFiles((prev) => [
      ...prev,
      createdFile,
    ]);

    setFileContents((prev) => ({
      ...prev,
      [createdFile]: content,
    }));

    setOpenFiles((prev) => [
      ...prev,
      createdFile,
    ]);

    setActiveFile(createdFile);
    setCode(content);
  } catch (error) {
    console.error("Failed to create file:", error);
    window.alert("Failed to create Gamma file.");
  }
}, [projectFiles]);


  const handleOpenFile = useCallback(
  async (fileName: string) => {
    try {
      // Save current file in React state
      setFileContents((prev) => ({
        ...prev,
        [activeFile]: code,
      }));

      // Read actual file from backend
      const response = await api.get(
        `/files/${encodeURIComponent(fileName)}`
      );

      const nextCode = response.data.content ?? "";

      setActiveFile(fileName);
      setCode(nextCode);

      setFileContents((prev) => ({
        ...prev,
        [fileName]: nextCode,
      }));

      setOpenFiles((prev) =>
        prev.includes(fileName)
          ? prev
          : [...prev, fileName]
      );
    } catch (error) {
      console.error("Failed to open file:", error);
      window.alert("Failed to open Gamma file.");
    }
  },
  [activeFile, code]
);


  const handleLoadExample = useCallback(
    (exampleCode: string) => {
      setCode(exampleCode);

      setFileContents((prev) => ({
        ...prev,
        [activeFile]: exampleCode,
      }));
    },
    [activeFile]
  );

  useEffect(() => {
  const loadProjectFiles = async () => {
    try {
      const response = await api.get("/files");

      let files: string[] = response.data.files ?? [];

      // First launch: create the default test.g file
      if (files.length === 0) {
        const createResponse = await api.post("/files", {
          name: "test.g",
        });

        const defaultContent = INITIAL_CODE;

        await api.put("/files/test.g", {
          content: defaultContent,
        });

        files = [createResponse.data.file];
      }

      setProjectFiles(files);

      const firstFile = files.includes("test.g")
        ? "test.g"
        : files[0];

      const fileResponse = await api.get(
        `/files/${encodeURIComponent(firstFile)}`
      );

      const content = fileResponse.data.content ?? "";

      setActiveFile(firstFile);
      setOpenFiles([firstFile]);
      setCode(content);

      setFileContents({
        [firstFile]: content,
      });
    } catch (error) {
      console.error(
        "Failed to load project files:",
        error
      );
    }
  };

  loadProjectFiles();
}, []);

useEffect(() => {
  if (!activeFile) {
    return;
  }

  const timer = window.setTimeout(async () => {
    try {
      await api.put(
        `/files/${encodeURIComponent(activeFile)}`,
        {
          content: code,
        }
      );
    } catch (error) {
      console.error(
        "Failed to save file:",
        error
      );
    }
  }, 500);

  return () => {
    window.clearTimeout(timer);
  };
}, [code, activeFile]);

  const compile = useCallback(async () => {
  setIsCompiling(true);

  const start = performance.now();

  try {
    const res = await api.post("/compile", { code });

    const elapsed = parseFloat(
      ((performance.now() - start) / 1000).toFixed(3)
    );

    setCompileTime(elapsed);
    setResult(res.data);

    if ((res.data.errors?.length ?? 0) > 0) {
      setCompileStatus("error");
      setActiveTab("errors");
    } else {
      setCompileStatus("success");
      setActiveTab("output");
    }
  } catch (error) {
    const elapsed = parseFloat(
      ((performance.now() - start) / 1000).toFixed(3)
    );

    setCompileTime(elapsed);

    setResult({
      output: "",
      tokens: [],
      ast: {
        type: "Program",
        children: [],
      },
      assembly: "",
      errors: [
        {
          line: 0,
          message: "Unable to connect to the compiler backend.",
          source: "",
          pointer: "",
        },
      ],
    });

    setCompileStatus("error");
    setActiveTab("errors");

    console.error("Compilation request failed:", error);
  } finally {
    setIsCompiling(false);
  }
}, [code]);

  const run = useCallback(async () => {
  setIsRunning(true);

  try {
    // Compile current source code first
    const compileResponse = await api.post("/compile", { code });

    setResult(compileResponse.data);

    const compileErrors = compileResponse.data.errors ?? [];

    if (
      compileResponse.data.success !== true ||
      compileErrors.length > 0
    ) {
      setCompileStatus("error");
      setActiveTab("errors");
      return;
    }

    setCompileStatus("success");

    // Start the compiled program
    const runResponse = await api.post("/run");

    if (
      runResponse.data.success !== true ||
      !runResponse.data.runId
    ) {
      throw new Error(
        runResponse.data.error || "Failed to start program."
      );
    }

    const runId = runResponse.data.runId;

    setActiveRunId(runId);
    setProgramInput("");
    console.log("ACTIVE RUN ID:", runId);

    // Read output until program finishes
    while (true) {
      const statusResponse = await api.get("/run/status", {
        params: { runId },
      });

      const status = statusResponse.data;

      setResult((prev) => ({
        ...prev,
        output: status.output ?? "",
      }));

      setActiveTab("output");

      if (status.finished) {
        setCompileStatus(
          status.exitCode === 0 ? "success" : "error"
        );

        setActiveRunId(null);

        break;
      }

      await new Promise((resolve) =>
        setTimeout(resolve, 250)
      );
    }
  } catch (err) {
    console.error("Run failed:", err);

    setCompileStatus("error");
    setActiveTab("errors");
  } finally {
    setIsRunning(false);
  }
}, [code]);
  const clear = useCallback(() => {
    setCode("");
    setResult({
      output: "",
      tokens: [],
      ast: { type: "Program", children: [] },
      assembly: "",
      errors: [],
    });
    setCompileStatus("idle");
  }, []);

  const linesOfCode = code.split("\n").filter(Boolean).length || 1;
  const tokenCount = result.tokens?.length ?? 0;
  const astNodes = result.ast ? countNodes(result.ast) : 0;

  return (
    <div
      className="flex flex-col h-screen overflow-hidden text-foreground"
      style={{ fontFamily: "'Inter', system-ui, sans-serif", backgroundColor: "#0D1117" }}
    >
      {/* ── Navbar ──────────────────────────────────────────────────── */}
      <nav
        className="flex items-center justify-between px-3 border-b border-white/10 h-12 shrink-0 z-50"
        style={{ backgroundColor: "#161B22" }}
      >
        {/* Left */}
        <div className="flex items-center gap-2 min-w-0">
          <button
            onClick={() => setSidebarOpen((p) => !p)}
            className="p-1.5 rounded hover:bg-white/10 transition-colors text-gray-400 hover:text-gray-200 shrink-0"
            title="Toggle Explorer"
          >
            <VscFiles size={17} />
          </button>
          <div className="flex items-center gap-2 shrink-0">
            <GammaLogo />
            <span className="font-semibold text-white text-sm hidden sm:block">
              Gamma Compiler
            </span>
          </div>
        </div>

        {/* Center */}
        <div
          className="flex items-center gap-1.5 px-3 py-1 rounded text-sm"
          style={{ backgroundColor: "#0D1117" }}
        >
          <VscFile size={13} className="text-blue-400 shrink-0" />
          <span className="text-gray-300 font-mono text-[13px]">
          {activeFile}
          </span>
        </div>

        {/* Right */}
        <div className="flex items-center gap-1.5">
          <button
            onClick={compile}
            disabled={isCompiling}
            className="flex items-center gap-1.5 px-3 py-1.5 rounded text-[12px] font-semibold transition-all disabled:opacity-50 hover:brightness-110 active:scale-95 whitespace-nowrap"
            style={{ backgroundColor: "#238636", color: "white" }}
          >
            <FiZap size={12} />
            <span className="hidden sm:inline">{isCompiling ? "Compiling…" : "Compile"}</span>
          </button>
          <button
            onClick={run}
            disabled={isRunning || isCompiling}
            className="flex items-center gap-1.5 px-3 py-1.5 rounded text-[12px] font-semibold transition-all disabled:opacity-50 hover:brightness-110 active:scale-95 whitespace-nowrap"
            style={{ backgroundColor: "#1F6FEB", color: "white" }}
          >
            <VscPlay size={13} />
            <span className="hidden sm:inline">{isRunning ? "Running…" : "Run"}</span>
          </button>
          <button
            onClick={clear}
            className="flex items-center gap-1.5 px-3 py-1.5 rounded text-[12px] font-semibold transition-all hover:brightness-110 active:scale-95"
            style={{ backgroundColor: "#30363D", color: "#E6EDF3" }}
          >
            <VscClearAll size={13} />
            <span className="hidden sm:inline">Clear</span>
          </button>
          <div className="w-px h-5 bg-white/10 mx-0.5" />
          <button
            className="p-1.5 rounded hover:bg-white/10 transition-colors text-gray-400 hover:text-gray-200"
            title="Settings"
          >
            <VscSettingsGear size={17} />
          </button>
          <button
            className="p-1.5 rounded hover:bg-white/10 transition-colors text-gray-400 hover:text-gray-200"
            title="GitHub"
          >
            <VscGithubInverted size={17} />
          </button>
        </div>
      </nav>

      {/* ── Body ────────────────────────────────────────────────────── */}
      <div className="flex flex-1 overflow-hidden">
        {/* ── Sidebar ────────────────────────────────────────── */}
        {sidebarOpen && (
          <div
            className="w-52 shrink-0 border-r border-white/10 overflow-hidden flex flex-col"
            style={{ backgroundColor: "#161B22" }}
          >
            <FileExplorer
              projectFiles={projectFiles}
              activeFile={activeFile}
              onOpenFile={handleOpenFile}
              onNewFile={handleNewFile}
              onLoadExample={handleLoadExample}
            />            
          </div>
        )}

        {/* ── Center: Editor + Bottom Panel ─────────────────── */}
        <div className="flex-1 flex flex-col overflow-hidden min-w-0">
          {/* Editor Tabs */}
        <div
          className="flex items-center h-9 shrink-0 border-b border-white/10 overflow-x-auto"
          style={{ backgroundColor: "#161B22" }}
        >
          {openFiles.map((fileName) => (
            <button
              key={fileName}
              onClick={() => handleOpenFile(fileName)}
              className={`flex items-center gap-2 px-3 h-full text-xs border-r border-white/10 transition-colors ${
                fileName === activeFile
                  ? "bg-[#1E1E1E] text-white"
                  : "text-gray-500 hover:text-gray-300 hover:bg-white/5"
              }`}
            >
              <VscFile
                size={13}
                className={
                  fileName === activeFile
                    ? "text-blue-400"
                    : "text-gray-600"
                }
              />

              <span>{fileName}</span>
            </button>
          ))}
        </div>

        {/* Monaco Editor */}
        <div className="flex-1 overflow-hidden">
            <Editor
              height="100%"
              defaultLanguage="gamma"
              language="gamma"
              path={activeFile}
              value={code}
              onChange={(val) => {
              const newCode = val ?? "";

              setCode(newCode);

              setFileContents((prev) => ({
                ...prev,
                [activeFile]: newCode,
              }));
            }}
              onMount={handleEditorMount}
              options={{
                fontSize: 15,
                fontFamily: "'JetBrains Mono', 'Fira Code', 'Cascadia Code', monospace",
                lineNumbers: "on",
                minimap: { enabled: true, scale: 1, renderCharacters: false },
                scrollBeyondLastLine: false,
                automaticLayout: true,
                tabSize: 4,
                autoIndent: "full",
                bracketPairColorization: { enabled: true },
                matchBrackets: "always",
                autoClosingBrackets: "always",
                autoClosingQuotes: "always",
                wordWrap: "off",
                renderLineHighlight: "line",
                smoothScrolling: true,
                cursorSmoothCaretAnimation: "on",
                padding: { top: 14, bottom: 14 },
                lineDecorationsWidth: 4,
                glyphMargin: false,
                folding: true,
                contextmenu: true,
                scrollbar: {
                  verticalScrollbarSize: 8,
                  horizontalScrollbarSize: 8,
                },
              }}
            />
          </div>

          {/* ── Bottom Panel ──────────────────────────────────── */}
          <div
            className="h-64 shrink-0 border-t border-white/10 flex flex-col overflow-hidden"
          >
            <OutputPanel
              result={result}
              activeTab={activeTab}
              onTabChange={setActiveTab}
              compileStatus={compileStatus}
              programInput={programInput}
              setProgramInput={setProgramInput}
              activeRunId={activeRunId}
            />
          </div>
        </div>

        {/* ── Right Panel ───────────────────────────────────── */}
        <div
          className="w-52 shrink-0 border-l border-white/10 flex flex-col overflow-y-auto"
          style={{ backgroundColor: "#161B22" }}
        >
          <div className="px-3 py-2 text-[11px] font-semibold text-gray-400 uppercase tracking-widest border-b border-white/5 shrink-0">
            Compiler Stats
          </div>

          <div className="p-3 flex flex-col gap-2.5">
            <StatCard
              icon={<FiClock size={13} />}
              label="Compile Time"
              value={`${compileTime}s`}
              colorClass="text-blue-400"
            />
            <StatCard
              icon={<FiCode size={13} />}
              label="Lines of Code"
              value={linesOfCode}
              colorClass="text-green-400"
            />
            <StatCard
              icon={<FiHash size={13} />}
              label="Tokens"
              value={tokenCount}
              colorClass="text-purple-400"
            />
            <StatCard
              icon={<FiLayers size={13} />}
              label="AST Nodes"
              value={astNodes}
              colorClass="text-yellow-400"
            />
            <StatCard
              icon={<FiAlertTriangle size={13} />}
              label="Warnings"
              value={0}
              colorClass="text-orange-400"
            />
            <StatCard
              icon={<FiXCircle size={13} />}
              label="Errors"
              value={result.errors?.length ?? 0}
              colorClass="text-red-400"
            />
          </div>

          <div className="px-3 pb-3">
            <div className="text-[11px] font-semibold text-gray-400 uppercase tracking-widest mb-2">
              Status
            </div>
            <div
              className={`text-xs font-mono px-2.5 py-2 rounded-md flex items-center gap-2 ${
                compileStatus === "success"
                  ? "text-green-400 bg-green-400/10 border border-green-400/20"
                  : compileStatus === "error"
                  ? "text-red-400 bg-red-400/10 border border-red-400/20"
                  : "text-gray-400 bg-white/5 border border-white/10"
              }`}
            >
              <span
                className={`w-2 h-2 rounded-full shrink-0 animate-pulse ${
                  compileStatus === "success"
                    ? "bg-green-400"
                    : compileStatus === "error"
                    ? "bg-red-400"
                    : "bg-gray-600"
                }`}
              />
              {compileStatus === "success"
                ? "Compiled OK"
                : compileStatus === "error"
                ? "Compile Error"
                : "Ready"}
            </div>
          </div>

          <div className="px-3 pb-4">
            <div className="text-[11px] font-semibold text-gray-400 uppercase tracking-widest mb-2">
              Example Programs
            </div>
            <div className="flex flex-col gap-1">
              {Object.entries(EXAMPLES).map(([label, exCode]) => (
                <button
                  key={label}
                  onClick={() => setCode(exCode)}
                  className="text-left text-[11px] px-2.5 py-1.5 rounded transition-colors text-gray-400 hover:text-gray-100 hover:bg-white/10 font-mono border border-transparent hover:border-white/10"
                >
                  {label}
                </button>
              ))}
            </div>
          </div>
        </div>
      </div>

      {/* ── Status Bar ──────────────────────────────────────────────── */}
      <div
        className="flex items-center justify-between px-4 h-6 text-[11px] text-white shrink-0 select-none"
        style={{ backgroundColor: "#1F6FEB" }}
      >
        <div className="flex items-center gap-4">
          <span className="font-bold flex items-center gap-1">
            <RiCpuLine size={12} />
            Gamma v1.0
          </span>
          <span className="opacity-75 hidden md:inline">Language: Gamma</span>
          <span className="opacity-75">Ln {linesOfCode}</span>
        </div>
        <div className="flex items-center gap-4 opacity-80">
          <span className="hidden sm:inline">UTF-8</span>
          <span className="hidden sm:inline">WSL</span>
          <span
            className={
              compileStatus === "success"
                ? "text-green-300 font-medium"
                : compileStatus === "error"
                ? "text-red-300 font-medium"
                : ""
            }
          >
            {compileStatus === "success"
              ? "✓ Ready"
              : compileStatus === "error"
              ? "✗ Error"
              : "Ready"}
          </span>
        </div>
      </div>
    </div>
  );
}