CC=gcc
CFLAGS=-Wall -ansi -pedantic -Isrc
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

OBJFILES=$(OBJDIR)/main.o $(OBJDIR)/pixmap.o $(OBJDIR)/bmp_writer.o


$(EXENAME):	$(OBJFILES)
	$(CC) -o $@ $^ $(LFLAGS)

$(OBJDIR)/main.o:	$(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/pixmap.o:	$(SRCDIR)/pixmap.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/bmp_writer.o:	$(SRCDIR)/exporters/bmp_writer.c
	$(CC) $(CFLAGS) -c $< -o $@
