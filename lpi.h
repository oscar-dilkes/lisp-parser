#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// max lines for file input
#define MAXLINES 30
// max line length for file input
#define MAXLINELENGTH 100

#define MAXTOKENS 1000
#define MAXTOKENLENGTH 50

// standard error message
#define ERROR(PHRASE) { fprintf(stderr, \
            "Fatal Error: %s occured in %s, line %d\n", PHRASE, \
            __FILE__, __LINE__); \
            exit(EXIT_FAILURE);}

// current character in input file
#define TOKENISERPOSITION                                                      \
  filestruct->filearray[filestruct->currentline][filestruct->currentposition]
// next character in input file
#define TOKENISERPEEK                                                          \
  filestruct->filearray[filestruct->currentline][filestruct->currentposition + 1]

#define PARSEPOSITION LISP->tokenstream[LISP->currenttoken]

// structure contains tokenized instruction set for parsing and interpreting
struct program {
  char tokenstream[MAXTOKENS][MAXTOKENLENGTH];
  int tokens;
  int currenttoken;
  int tokenposition;
};

// structure used to literal input file and contains information used for
// tokenization
struct file {
  char filearray[MAXLINES][MAXLINELENGTH];
  int currentline;
  int currentposition;
};

// enum typename { var, lit, nil, ret }

struct ast {
  int value1;
  int value2;
  // enum typename type
  bool less; bool greater; bool equal;
  // char*
};

// main tokeniser function, recognises characters and redirects to corresponding
// functions, returns the final array containing the tokens, or exits upon
// syntax error
void tokeniser(struct file *filestruct, struct program *LISP);
// places parenthesis token in current line
void parenthesis(struct file *filestruct, struct program *LISP);
// places letters and/or strings (functions according to grammar) in current line
void letter(struct file *filestruct, struct program *LISP);
// places a literal in current line
void apostrophe(struct file *filestruct, struct program *LISP);
// places strings in current line
void quotation(struct file *filestruct, struct program *LISP);
// at the start of a new line locates where instructions begin
void locate(struct file *filestruct);

// functions for each line of grammar
void prog(struct program *LISP, struct ast *interp);
void instrcts(struct program *LISP, struct ast *interp);
void instrct(struct program *LISP, struct ast *interp);
void func(struct program *LISP, struct ast *interp);
void retfunc(struct program *LISP, struct ast *interp);
void listfunc(struct program *LISP, struct ast *interp);
void intfunc(struct program *LISP, struct ast *interp);
void boolfunc(struct program *LISP, struct ast *interp);
void iofunc(struct program *LISP, struct ast *interp);
void set(struct program *LISP, struct ast *interp);
void print(struct program *LISP, struct ast *interp);
// _parse added to distinguish from 'if'
void if_parse(struct program *LISP, struct ast *interp);
void loop(struct program *LISP, struct ast *interp);
void list(struct program *LISP, struct ast *interp);
void var(struct program *LISP, struct ast *interp);
void string(struct program *LISP, struct ast *interp);
void literal(struct program *LISP, struct ast *interp);
