CC      := clang
DEBUG   := -g3
OPTIM   := -O3
CFLAGS  := -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99 -Werror
RELEASE := $(CFLAGS) $(OPTIM)
SANI    := $(CFLAGS) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALG    := $(CFLAGS)  $(DEBUG)
NCLS    := $(wildcard *.ncl)
PRES := $(NCLS:.ncl=.pres)
IRES := $(NCLS:.ncl=.ires)
LIBS    := -lm


parse: lpi.c general.c lpi.h
	$(CC) lpi.c general.c $(RELEASE) -o parse $(LIBS)

parse_s: lpi.c general.c lpi.h
	$(CC) lpi.c general.c $(SANI) -o parse_s $(LIBS)

parse_v: lpi.c general.c lpi.h
	$(CC) lpi.c general.c $(VALG) -o parse_v $(LIBS)

all: parse parse_s parse_v interp interp_s interp_v

interp: lpi.c lisp.c general.c lpi.h lisp.h general.h
	$(CC) lpi.c lisp.c general.c $(RELEASE) -DINTERP -o interp $(LIBS)

interp_s: lpi.c lisp.c general.c lpi.h lisp.h general.h
	$(CC) lpi.c lisp.c general.c $(SANI) -DINTERP -o interp_s $(LIBS)

interp_v: lpi.c lisp.c general.c lpi.h lisp.h general.h
	$(CC) lpi.c lisp.c general.c $(VALG) -DINTERP -o interp_v $(LIBS)

runall : ./parse_s ./interp_s $(PRES) $(IRES)

%.pres:
	-./parse_s  $*.ncl > $*.pres 2>&1
%.ires:
	-./interp_s $*.ncl > $*.ires 2>&1

clean:
	rm -f parse parse_s parse_v interp interp_s interp_v $(PRES) $(IRES)
	