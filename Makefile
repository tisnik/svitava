CC=gcc
CFLAGS=-Wall -ansi -pedantic -Isrc
LFLAGS=-lm -lpng

SRCDIR=src
OBJDIR=obj
EXENAME=svitava

all:	$(EXENAME)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(EXENAME)

run:	$(EXENAME)
	./$(EXENAME)

OBJFILES=$(OBJDIR)/main.o $(OBJDIR)/pixmap.o $(OBJDIR)/bmp_writer.o $(OBJDIR)/ppm_writer.o $(OBJDIR)/png_writer.o $(OBJDIR)/mandelbrot.o


$(EXENAME):	$(OBJFILES)
	$(CC) -o $@ $^ $(LFLAGS)

$(OBJDIR)/main.o:	$(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/pixmap.o:	$(SRCDIR)/pixmap.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/bmp_writer.o:	$(SRCDIR)/exporters/bmp_writer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/ppm_writer.o:	$(SRCDIR)/exporters/ppm_writer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/png_writer.o:	$(SRCDIR)/exporters/png_writer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/mandelbrot.o:	$(SRCDIR)/renderers/mandelbrot.c
	$(CC) $(CFLAGS) -c $< -o $@
