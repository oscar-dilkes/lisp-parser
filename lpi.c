#include "nuclei.h"
#include "lisp.h"
#include "specific.h"

int main(int argc, char *argv[]) {
  struct file *filestruct = (struct file *)calloc(1, sizeof(struct file));

  // tests to check for correct number of arguments (2)
  // assert(argc == 2);
  if (argc < 2) {
    free(filestruct);
    filestruct = NULL;
    ERROR("Too few arguments.");
  }
  if (argc > 2) {
    free(filestruct);
    filestruct = NULL;
    ERROR("Too many arguments.");
  }

  char *filename = argv[1];

  FILE *fp;

  fp = fopen(filename, "r");

  // test to check for empty file
  if (fp == NULL) {
    free(filestruct);
    filestruct = NULL;
    ERROR("Failed to open file.")
  }

  assert(fp);

  int lines = 0;

  // file input line by line until end of file reached
  while (!feof(fp)) {
    // exits if file read is problematic
    if (ferror(fp)) {
      free(filestruct);
      filestruct = NULL;
      ERROR("Problem reading file.")
    }
    fgets(filestruct->filearray[lines], MAXLINELENGTH, fp);
    lines++;
  }

  fclose(fp);

  // corrects value of lines
  lines--;

  // // print file for debugging
  // for (int i = 0; i < lines; i++) {
  //   printf("%s", filestruct->filearray[i]);
  // }

  struct program *LISP = (struct program *)calloc(1, sizeof(struct program));

  tokeniser(filestruct, LISP);

  // // print file for debugging
  // for (int i = 0; i < LISP->tokens; i++) {
  //   printf("%s\n", LISP->tokenstream[i]);
  // }

  struct ast *interp = (struct ast *)calloc(1, sizeof(struct ast));

  prog(LISP, interp);

  // // print file for debugging
  // for (int i = 0; i < LISP->tokens; i++) {
  //   printf("%s\n", LISP->tokenstream[i]);
  // }

  printf("Parsed OK\n");

  free(filestruct);
  filestruct = NULL;
  free(LISP);
  LISP = NULL;
  free(interp);
  interp = NULL;

  assert(!filestruct);
  assert(!LISP);
  assert(!interp);

  return 0;
}

void tokeniser(struct file *filestruct, struct program *LISP) {
  // moves to the following line if the next character is a newline (10) or hash
  // (denotes a comment)
  if (TOKENISERPOSITION == 10 || TOKENISERPOSITION == '#') {
    filestruct->currentline++;
    filestruct->currentposition = 0;
    assert(filestruct->currentline != 0);
    // checks for end of file and returns
    if (TOKENISERPOSITION == 0) {
      assert(filestruct->filearray[filestruct->currentline - 1][filestruct->currentposition]);
      assert(LISP->tokenstream[LISP->tokens - 1][LISP->tokenposition]);
      return;
    }
  }
  // locates next character if tokeniser encounters a space (32)
  if (TOKENISERPOSITION == 32) {
    locate(filestruct);
    // adjusts position
    filestruct->currentposition--;
    assert(filestruct->currentposition >= 0);
  }
  if (TOKENISERPOSITION == '(' || TOKENISERPOSITION == ')') {
    assert(TOKENISERPOSITION == '(' || TOKENISERPOSITION == ')');
    parenthesis(filestruct, LISP);
  } else if (isalpha(TOKENISERPOSITION)) {
    assert(!isdigit(TOKENISERPOSITION));
    letter(filestruct, LISP);
  }
  // use ASCII value of apostrophe (39) since it cannot be contained in
  // single quotes
  else if (TOKENISERPOSITION == 39) {
    apostrophe(filestruct, LISP);
  }
  // use ASCII value of quotation mark (34) since it cannot be contained in
  // single quotes
  else if (TOKENISERPOSITION == 34) {
    quotation(filestruct, LISP);
  } else {
    // exit upon discovery of unrecognised or invalid character
    free(filestruct);
    filestruct = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Syntax error.")
  }
}

void parenthesis(struct file *filestruct, struct program *LISP) {
  if (TOKENISERPOSITION == '(') {
    LISP->tokenstream[LISP->tokens][LISP->tokenposition] = '(';
    assert(LISP->tokenstream[LISP->tokens][LISP->tokenposition] != ')');
  } else {
    LISP->tokenstream[LISP->tokens][LISP->tokenposition] = ')';
    assert(LISP->tokenstream[LISP->tokens][LISP->tokenposition] != '(');
  }
  // appends tokens to move to next line in array (next token)
  LISP->tokens++;
  // resets position to start of next line in array (next token)
  LISP->tokenposition = 0;
  // appends position to move to next character in the input file
  filestruct->currentposition++;
  tokeniser(filestruct, LISP);
}

void letter(struct file *filestruct, struct program *LISP) {
  // places letter down at start because status as a letter is verified through
  // all control paths
  LISP->tokenstream[LISP->tokens][LISP->tokenposition] = TOKENISERPOSITION;
  if (TOKENISERPEEK == ' ' || TOKENISERPEEK == ')') {
    LISP->tokens++;
    LISP->tokenposition = 0;
    filestruct->currentposition++;
    assert(LISP->tokenposition == 0);
    tokeniser(filestruct, LISP);
  } else if (isalpha(TOKENISERPEEK)) {
    // appends tokenposition to insert next letter
    LISP->tokenposition++;
    filestruct->currentposition++;
    letter(filestruct, LISP);
  } else {
    // letters can only be followed by other letters, a space, or a close
    // bracket
    free(filestruct);
    filestruct = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Syntax error.")
  }
}

void apostrophe(struct file *filestruct, struct program *LISP) {
  // places apostrophe or character down as if apostrophe this is verified
  // through all control paths and assessment instructions say not to check
  // contents of literals
  LISP->tokenstream[LISP->tokens][LISP->tokenposition] = TOKENISERPOSITION;
  // see if the following token is the corresponding close apostrophe
  if (TOKENISERPEEK == 39) {
    LISP->tokenstream[LISP->tokens][LISP->tokenposition + 1] = 39;
    LISP->tokens++;
    LISP->tokenposition = 0;
    filestruct->currentposition = filestruct->currentposition + 2;
    assert(LISP->tokenposition == 0);
    tokeniser(filestruct, LISP);
  } else {
    LISP->tokenposition++;
    filestruct->currentposition++;
    apostrophe(filestruct, LISP);
  }
}

// identical function to apostrophe so commments same
void quotation(struct file *filestruct, struct program *LISP) {
  LISP->tokenstream[LISP->tokens][LISP->tokenposition] = TOKENISERPOSITION;
  if (TOKENISERPEEK == 34) {
    LISP->tokenstream[LISP->tokens][LISP->tokenposition + 1] = 34;
    LISP->tokens++;
    LISP->tokenposition = 0;
    filestruct->currentposition = filestruct->currentposition + 2;
    assert(LISP->tokenposition == 0);
    tokeniser(filestruct, LISP);
  } else {
    LISP->tokenposition++;
    filestruct->currentposition++;
    quotation(filestruct, LISP);
  }
}

void locate(struct file *filestruct) {
  // already know current character is a space since that is a requirement to
  // call this function
  char charcurrent = ' ';
  while (charcurrent == ' ') {
    assert(charcurrent == ' ');
    charcurrent =
        filestruct
            ->filearray[filestruct->currentline][filestruct->currentposition];
    filestruct->currentposition++;
  }
  return;
}

void prog(struct program *LISP, struct ast *interp) {
  if (strcmp(PARSEPOSITION, "(")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("No opening bracket?")
  }
  LISP->currenttoken++;
  assert(LISP->currenttoken > 0);
  // while loop ensures that parsing/interpreting continues until the end of the file is reached
  while (LISP->currenttoken < LISP->tokens) {
    // if coming back here from further parsing, adjusts the position in the file to the next token
    while (strcmp(PARSEPOSITION, "(")) {
      LISP->currenttoken++;
      // double check for end of file
      if (PARSEPOSITION[0] == 0) {
        return;
      }
    }
    instrcts(LISP, interp);
  }
  return;
}
void instrcts(struct program *LISP, struct ast *interp) {
  //begin bracket counting at one because reaching this function requires previous position to be an open bracket
  int bracketcounter = 1;
  int bracketposition = LISP->currenttoken;
  while (bracketcounter != 0) {
    // handles case where bracket counting continues into blank array space beyond token list
    if (bracketposition > LISP->tokens && bracketcounter != 0) {
      free(interp);
      interp = NULL;
      free(LISP);
      LISP = NULL;
      ERROR("Couldn't locate a corresponding closing bracket?");
    }
    // append count on encountering open bracket
    if (!strcmp(LISP->tokenstream[bracketposition], "(")) {
      bracketcounter++;
      bracketposition++;
    // remove from count on encountering close bracket
    } else if (!strcmp(LISP->tokenstream[bracketposition], ")")) {
      bracketcounter--;
      bracketposition++;
    } else {
    // if token isn't a bracket ignore count and move to next token
      bracketposition++;
    }
  }
  assert(bracketcounter == 0);
  instrct(LISP, interp);
  return;
}
void instrct(struct program *LISP, struct ast *interp) {
  if (strcmp(PARSEPOSITION, "(")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("No opening bracket?");
  }
  LISP->currenttoken++;
  // grammar tells us function call will always be an uppercase word
  if (!isupper(PARSEPOSITION[0])) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise function?");
  }
  func(LISP, interp);
}
void func(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "CAR") || !strcmp(PARSEPOSITION, "CDR") ||
      !strcmp(PARSEPOSITION, "CONS") || !strcmp(PARSEPOSITION, "PLUS") ||
      !strcmp(PARSEPOSITION, "LENGTH") || !strcmp(PARSEPOSITION, "LESS") ||
      !strcmp(PARSEPOSITION, "GREATER") || !strcmp(PARSEPOSITION, "EQUAL")) {
    retfunc(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "SET") || !strcmp(PARSEPOSITION, "PRINT")) {
    iofunc(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "IF")) {
    if_parse(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "WHILE")) {
    loop(LISP, interp);
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise function?");
  }
  return;
}

void retfunc(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "CAR") || !strcmp(PARSEPOSITION, "CDR") ||
      !strcmp(PARSEPOSITION, "CONS")) {
    listfunc(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "PLUS") ||
             !strcmp(PARSEPOSITION, "LENGTH")) {
    intfunc(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "LESS") ||
             !strcmp(PARSEPOSITION, "GREATER") ||
             !strcmp(PARSEPOSITION, "EQUAL")) {
    boolfunc(LISP, interp);
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise return function?");
  }
  return;
}
void listfunc(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "CAR") || !strcmp(PARSEPOSITION, "CDR")) {
    LISP->currenttoken++;
    list(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "CONS")) {
    LISP->currenttoken++;
    list(LISP, interp);
    LISP->currenttoken++;
    list(LISP, interp);
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise list function?");
  }
  return;
}
void intfunc(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "PLUS")) {
    LISP->currenttoken++;
    list(LISP, interp);
    LISP->currenttoken++;
    list(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "LENGTH")) {
    LISP->currenttoken++;
    list(LISP, interp);
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise integer function?");
  }
  return;
}
void boolfunc(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "LESS") || !strcmp(PARSEPOSITION, "GREATER") ||
      !strcmp(PARSEPOSITION, "EQUAL")) {
    LISP->currenttoken++;
    list(LISP, interp);
    LISP->currenttoken++;
    list(LISP, interp);
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise boolean function?")
  }
  return;
}

void iofunc(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "SET")) {
    LISP->currenttoken++;
    set(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "PRINT")) {
    LISP->currenttoken++;
    print(LISP, interp);
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Couldn't recognise io function?");
  }
  return;
}
void set(struct program *LISP, struct ast *interp) {
  var(LISP, interp);
  LISP->currenttoken++;
  list(LISP, interp);
  LISP->currenttoken++;
}
void print(struct program *LISP, struct ast *interp) {
  // checks for opening quotation mark to verify string
  if (PARSEPOSITION[0] == 34) {
    string(LISP, interp);
  // handles if neither string or list within list function call
  } else {
    list(LISP, interp);
  }
  return;
}

void if_parse(struct program *LISP, struct ast *interp) {
  if (strcmp(PARSEPOSITION, "IF")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting 'IF' at start of statement?");
  }
  LISP->currenttoken++;
  if (strcmp(PARSEPOSITION, "(")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting opening bracket at start of statement?");
  }
  LISP->currenttoken++;
  boolfunc(LISP, interp);
  LISP->currenttoken++;
  if (strcmp(PARSEPOSITION, ")")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting closing bracket at end of statement?");
  }
  LISP->currenttoken++;
  if (strcmp(PARSEPOSITION, "(")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting opening bracket at start of if-true instruction?");
  }
  LISP->currenttoken++;
  instrcts(LISP, interp);
  // iterate an additional two to skip past double close brackets
  LISP->currenttoken = LISP->currenttoken + 3;
  if (strcmp(PARSEPOSITION, "(")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting opening bracket at start of if-false instruction?");
  }
  LISP->currenttoken++;
  instrcts(LISP, interp);
  LISP->currenttoken++;
#ifdef INTERP
  if ()
#endif
    return;
}
void loop(struct program *LISP, struct ast *interp) {
  if (strcmp(PARSEPOSITION, "WHILE")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting 'WHILE' at start of loop?");
  }
  LISP->currenttoken++;
  if (strcmp(PARSEPOSITION, "(")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting opening bracket at start of loop?");
  }
  LISP->currenttoken++;
  boolfunc(LISP, interp);
  LISP->currenttoken++;
  if (strcmp(PARSEPOSITION, ")")) {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Was expecting closing bracket at end of loop?");
  }
  return;
}
void list(struct program *LISP, struct ast *interp) {
  if (!strcmp(PARSEPOSITION, "(")) {
    LISP->currenttoken++;
    retfunc(LISP, interp);
    LISP->currenttoken++;
    if (!strcmp(PARSEPOSITION, ")")) {
      return;
    } else {
      free(interp);
      interp = NULL;
      free(LISP);
      LISP = NULL;
      ERROR("Was expecing closing bracket at end of return function?");
    }
    // isupper to verify variable, need also to verify it isn't 'NIL'
  } else if (isupper(PARSEPOSITION[LISP->tokenposition]) &&
             strcmp(PARSEPOSITION, "NIL")) {
    var(LISP, interp);
  } else if (PARSEPOSITION[0] == 39) {
    literal(LISP, interp);
  } else if (!strcmp(PARSEPOSITION, "NIL")) {
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Invalid list.")
  }
  return;
}
void var(struct program *LISP, struct ast *interp) {
  if (isupper(PARSEPOSITION[LISP->tokenposition]) &&
      strlen(PARSEPOSITION) == 1) {
#ifdef INTERP
    interp->value1 = interp->value2;
    interp->value2 = PARSEPOSITION[LISP->tokenposition + 1];
    // sscanf(PARSEPOSITION, %i, &interp->value2);
#endif
    return;
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Invalid variable.")
  }
}
void string(struct program *LISP, struct ast *interp) {
  // checks for open and close quotation marks
  if (PARSEPOSITION[LISP->tokenposition] == 34 &&
      PARSEPOSITION[strlen(PARSEPOSITION) - 1] == 34) {
    return;
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("String incorrectly formatted?")
  }
}
void literal(struct program *LISP, struct ast *interp) {
  if (PARSEPOSITION[LISP->tokenposition] == 39 &&
      PARSEPOSITION[strlen(PARSEPOSITION) - 1] == 39) {
#ifdef INTERP
    interp->value1 = interp->value2;
    interp->value2 = PARSEPOSITION[LISP->tokenposition + 1];
    // sscanf(PARSEPOSITION, %i, &interp->value2);
#endif
    return;
  } else {
    free(interp);
    interp = NULL;
    free(LISP);
    LISP = NULL;
    ERROR("Literal incorrectly formatted?")
  }
}
