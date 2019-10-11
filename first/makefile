IDIR =./
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./
LDIR =./

LIBS=-lm

_DEPS = fsm.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = fsm.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fsm: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
