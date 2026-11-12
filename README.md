# Gamma Compiler

**A Custom Programming Language Compiler with a Browser-Based Integrated Development Environment (IDE)**

## Overview

Gamma Compiler is a custom programming language compiler developed using C++. It translates programs written in the Gamma programming language into x86-64 assembly language and produces executable programs.

The project also provides a browser-based Integrated Development Environment (IDE) that allows users to write, compile, and execute Gamma programs through a web interface.

## Key Features

- Custom Gamma programming language.
- Lexical analysis and syntax analysis.
- Abstract Syntax Tree (AST) representation.
- Code generation targeting x86-64 assembly.
- Support for variables and data types.
- Arithmetic, relational, and logical operators.
- Conditional statements and loops.
- Functions and arrays.
- String operations and pointers.
- NASM assembly and executable linking.
- Browser-based code editor.
- Compilation and execution interface.
- Interactive program input and output.
- Error reporting and compilation metadata.

## System Architecture

Gamma Compiler consists of three major components:

1. **Compiler:** Processes Gamma source code through tokenization, parsing, AST construction, and code generation.
2. **Backend:** Flask-based Python application responsible for compiler integration, compilation requests, execution management, and communication with the frontend.
3. **Frontend:** React and TypeScript-based browser IDE for writing, compiling, and executing Gamma programs.

## Technology Stack

| Component | Technology |
|---|---|
| Compiler | C++ |
| Assembly Generation | x86-64 NASM |
| Build System | CMake |
| Backend | Python, Flask |
| Frontend | React, TypeScript |
| Frontend Build Tool | Vite |
| Development Environment | Windows and WSL/Linux |

## Project Structure

```text
Gamma-Compiler/
├── src/
│   ├── main.cpp
│   ├── tokenization.hpp
│   ├── parser.hpp
│   ├── generation.hpp
│   ├── arena.hpp
│   └── runtime.asm
│
├── backend/
│   ├── app.py
│   └── requirements.txt
│
├── frontend/
│   ├── src/
│   ├── package.json
│   └── package-lock.json
│
├── docs/
├── examples/
├── gamma_files/
├── CMakeLists.txt
└── README.md
```

## Prerequisites

- C++ compiler compatible with the project.
- CMake.
- NASM assembler.
- Linux environment or WSL.
- Python and pip.
- Node.js and npm.

## Installation and Setup

### 1. Clone the Repository

```bash
git clone https://github.com/rohan388-rgb/Gamma-Compiler.git
cd Gamma-Compiler
```

### 2. Build the Compiler

Use the project's CMake configuration to configure and build the compiler.

### 3. Set Up the Backend

```bash
cd backend
pip install -r requirements.txt
```

### 4. Set Up the Frontend

```bash
cd frontend
npm install
```

### 5. Run the Application

Start the Flask backend and React frontend using the development commands configured in the project.

Refer to the frontend package scripts and backend configuration for the exact commands and required environment settings.

## Repository

GitHub: https://github.com/rohan388-rgb/Gamma-Compiler

## Contributors

- Rohan – Compiler Development
- Akanksha – Browser-Based IDE and Backend Integration

## License

No open-source license has currently been added to this repository.
