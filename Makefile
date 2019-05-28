CC=gcc
CFLAGS=-Wall -ansi -pedantic
LFLAGS=-lm

SRCDIR=src
OBJDIR=obj
EXENAME=svitava

all:	$(EXENAME)

clean:
	rm $(OBJDIR)/*.o
	rm $(EXENAME)

run:	$(EXENAME)
	./$(EXENAME)

OBJFILES=$(OBJDIR)/main.o


$(EXENAME):	$(OBJFILES)
	$(CC) -o $@ $^ $(LFLAGS)

$(OBJDIR)/main.o:	$(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@
