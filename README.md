# LISP Parser and Interpreter (WIP)

This project is a LISP parser and interpreter written in C, aiming to provide a simple yet functional LISP environment. The project is currently in development, with a focus on interpreting LISP expressions; below is a detailed overview of the project's current structure and functionality.

## Project Structure

The project is structured into several key components, each residing in its own file to maintain clarity and modularity:

- `general.h` and `general.c`: These files contain general utility functions used throughout the project, such as error handling (`on_error`), memory allocation wrappers (`ncalloc`, `n2dcalloc`, etc.), and file operations (`nfopen`).

- `lisp.h` and `lisp.c`: These define the core LISP data structures and functions for creating and manipulating LISP expressions, such as `lisp_atom`, `lisp_cons`, `lisp_car`, `lisp_cdr`, and more.

- `specific.h`: Contains specific definitions for the LISP implementation, including the `lisp` structure itself and constants such as `TESTVALUE`, `LISPIMPL`, and `STRLEN`.

- `lpi.h` and `lpi.c`: The LISP parser and interpreter files. They define the grammar for the LISP language, the tokeniser (`tokeniser`), and functions for parsing (`prog`, `instrcts`, `instrct`, etc.) and interpreting LISP code.

- `main` function (in `lpi.c`): Initiates file reading, tokenising, parsing, and interpreting processes, handling input files and preparing the environment for LISP code execution.

## Key Features

### General Utilities (`general.h/c`)

- **Error Handling**: Centralised error reporting function `on_error` for standardized error messaging.
- **Memory Management**: Wrappers around standard allocation functions to include error checking and provide additional functionality, such as 2D array allocation and resizing.

### LISP Core (`lisp.h/c`)

- **Data Structures**: Implementation of the LISP cons cell (`lisp` struct) for constructing complex data structures.
- **Core Functions**: Set of functions to create atoms (`lisp_atom`), construct lists (`lisp_cons`), and perform basic list operations (`lisp_car`, `lisp_cdr`).
- **Utility Functions**: Functions for deep copying (`lisp_copy`), determining the length of lists (`lisp_length`), converting lists to strings (`lisp_tostring`), and freeing allocated memory (`lisp_free`).

### Parser and Interpreter (`lpi.h/c`)

- **Tokenizing**: The `tokeniser` function breaks down input files into tokens recognizable by the parser, handling different types of tokens (parentheses, literals, strings, etc.).
- **Parsing**: Functions such as `prog`, `instrcts`, and `instrct` parse the tokenised input according to the defined grammar, building an abstract syntax tree (AST) for interpretation.
- **Interpreting**: The interpretation process is designed to execute the parsed LISP code, although this is incomplete thus far.

## Testing

Initial testing functions are included within `lisp.c` and `lpi.c`, to ensure proper creation, execution, and memory cleanup.

## Future Directions

- **Interpreter Development**: Continued development of the interpreter to support a wider range of LISP expressions and functionalities.
- **Error Handling**: Enhancements to error reporting and handling within the parser and interpreter to improve robustness and user feedback.
