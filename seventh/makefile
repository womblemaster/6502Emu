IDIR =./
CC=gcc
CFLAGS=-g3 -I$(IDIR)

ODIR=./
LDIR =./

LIBS=-lm -lncurses

_DEPS = fsm.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS),$(LIBS))

_OBJ = fsm.o inst.o addr.o memconfig.o assmbl.o dassmbl.o IO.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

%.o: %.c $(DEPS)
	$(CC) -O3 -c -o $@ $< $(CFLAGS)

fsm: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
