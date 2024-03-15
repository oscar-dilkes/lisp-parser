#include "specific.h"

lisp *lisp_atom(const atomtype a) {
  lisp *l = (lisp *)malloc(sizeof(lisp));

  l->value = a;
  l->car = NULL;
  l->cdr = NULL;
  l->length = 0;

  return l;
}

lisp *lisp_cons(const lisp *l1, const lisp *l2) {
  lisp *l = (lisp *)malloc(sizeof(lisp));

  l->car = (lisp *)l1;
  l->cdr = (lisp *)l2;

  // length handled throughout construction
  // provides smaller length function
  if (!l2) {
    l->length = 1;
  } else {
    l->length = (l2->length) + 1;
  }
  return l;
}

lisp *lisp_car(const lisp *l) { return l->car; }

lisp *lisp_cdr(const lisp *l) { return l->cdr; }

atomtype lisp_getval(const lisp *l) {
  if (lisp_isatomic(l)) {
    return l->value;
  } else {
    return lisp_getval(lisp_car(l));
  }
}

bool lisp_isatomic(const lisp *l) {
  if (!l) {
    return false;
  }
  if (!lisp_car(l)) {
    return true;
  } else {
    return false;
  }
}

lisp *lisp_copy(const lisp *l) {
  lisp *l_copy = (lisp *)malloc(sizeof(lisp));
  l_copy->length = l->length;
  if (lisp_isatomic(l)) {
    l_copy->car = NULL;
    l_copy->cdr = NULL;
    l_copy->value = l->value;
    return l_copy;
  }
  l_copy->car = lisp_copy(lisp_car(l));
  if (!l->cdr) {
    l_copy->cdr = NULL;
    return l_copy;
  } else {
    l_copy->cdr = lisp_copy(lisp_cdr(l));
    return l_copy;
  }
}

int lisp_length(const lisp *l) {
  // length tallied throughout construction of lists
  if (!l) {
    return 0;
  } else {
    return l->length;
  }
}

void lisp_tostring(const lisp *l, char *str) {
  for (int i = 0; i < STRLEN; i++) {
    str[i] = 0;
  }
  str[0] = '('; 
  str[1] = ')';

  if (!l) {
    return;
  }

  // atomic unit, print to 0 index
  if (lisp_isatomic(l)) {
    // ensure clear string for atomic unit
    for (int i = 0; i < STRLEN; i++) {
      str[i] = 0;
    }
    int value = l->value;
    // this creates memory issue
    sprintf(&str[0], "%i", value);
    return;
  }

  char tempstr[STRLEN + 1];
  lisp_tostring(lisp_car(l), tempstr);
  int lengthcar = strlen(tempstr);
  for (int i = 0; i < lengthcar; i++) {
    // moves the bracket along
    str[i + 2] = str[i + 1];
    // places tostring of car
    str[i + 1] = tempstr[i];
  }

  if (lisp_cdr(l)) {
    // add space if cdr element present (in case of following atom)
    str[lengthcar + 1] = ' ';
    lengthcar = strlen(str);
    // clear tempstr for re-use
    for (int i = 0; i < (STRLEN + 1); i++) {
      tempstr[i] = 0;
    }
    lisp_tostring(lisp_cdr(l), tempstr);
    int lengthcdr = strlen(tempstr);
    for (int i = 0; i < lengthcdr; i++) {
      // places tostring of cdr
      str[lengthcar + i] = tempstr[i + 1];
    }
  }

  return;
}

void lisp_free(lisp **l) {
  free(*l);
  *l = NULL;
}

lisp *lisp_fromstring(const char *str) {
  lisp *l = (lisp *)malloc(sizeof(lisp));
  const char *dummy;
  dummy = str;
  return l;
  free(l);
}

lisp *lisp_list(const int n, ...) {
  lisp *l = (lisp *)malloc(sizeof(lisp));
  l->value = n;
  return l;
  free(l);
}

void lisp_reduce(void (*func)(lisp *l, atomtype *n), lisp *l, atomtype *acc) {
  func(l, acc);
  return;
}

void test() {

  lisp *test_atom = lisp_atom(TESTVALUE);
  assert(test_atom->value);
  assert(!test_atom->car);
  assert(!test_atom->cdr);

  lisp *test_cons = lisp_cons(test_atom, NULL);
  assert(test_cons->car);

  assert(lisp_car(test_cons));

  assert(lisp_getval(test_atom));
  assert(lisp_getval(test_cons));

  assert(lisp_isatomic(test_atom) == true);
  assert(lisp_isatomic(test_cons) == false);

  lisp *test_copy = lisp_copy(test_cons);
  char str[STRLEN];
  char str_copy[STRLEN];
  test_copy = lisp_copy(test_cons);
  lisp_tostring(test_cons, str);
  lisp_tostring(test_copy, str_copy);
  assert(strcmp(str, str_copy) == 0);

  lisp_free(&test_atom);
  lisp_free(&test_cons);
  lisp_free(&test_copy);

  assert(!test_atom);
  assert(!test_cons);
  assert(!test_copy);
}
