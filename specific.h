#include "lisp.h"

struct lisp {
    struct lisp *cdr;
    struct lisp *car;
    atomtype value;
    int length;
};

// value used for assertion testing
#define TESTVALUE 2

#define LISPIMPL "LINKED"

// maximum string length
#define STRLEN 1000
