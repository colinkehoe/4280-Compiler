# CS 4280: Program Translation Techniques

This repository contains the full compiler for UMSL CS-4280, a multi-pass system designed to translate a custom high-level language into functional UMSL VirtMach assembly. Developed as a final project, it focuses on rigorous syntax analysis and efficient code generation.

---

## 🏗 Pipeline Architecture

The P5 compiler follows a traditional modular pipeline to ensure clean separation of concerns:

1.  **Scanner (Lexical Analysis):** Performs tokenization of the P5 source code, handling keywords, identifiers, and literals.
2.  **Parser (Syntax Analysis):** Validates the token stream against the formal language grammar to construct a robust Abstract Syntax Tree (AST).
3.  **Semantic Analyzer:** Ensures logical consistency through type checking and scope resolution.
4.  **Code Generator:** Traverses the AST to emit x86_64 assembly instructions, handling stack frames and register allocation.

---

## ✨ Key Features

*   **Custom Architecture:** Built to handle the unique specifications of the P5 language.
*   **Static Type System:** Includes a semantic pass to catch type mismatches during compilation.
*   **UMSL VirtMach Target:** Generates assembly compatible with standard assemblers like NASM or GAS.
*   **Error Reporting:** Clear feedback for syntax errors discovered during the parsing phase.

---

## 🛠 Technical Stack

*   **Implementation Language:** C++
*   **Build System:** CMake
*   **Assembly Target:** UMSL VirtMach
*   **Documentation:** LaTeX (Formal specifications and grammar)

---

## 🚀 Getting Started

### Prerequisites
*   A C++ compiler (GCC/Clang)
*   CMake 3.15+
*   `make` (to assemble and link the generated output)

### Installation
1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/your-username/P5-Compiler.git](https://github.com/your-username/P5-Compiler.git)
    cd P5-Compiler
