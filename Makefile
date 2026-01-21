CC=gcc
CFLAGS=-Wall -ansi -pedantic
#LFLAGS=-lm -lpng
LFLAGS=-lm

EXENAME=svitava
TESTNAME=test

all:	$(EXENAME)

clean:
	rm -f $(EXENAME)
	rm -f test
	rm -f *.gcno
	rm -f *.gcda
	rm -f coverage.*

run:	$(EXENAME)
	./$(EXENAME)

$(EXENAME):	svitava.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

$(TESTNAME):	test.c svitava.c
	$(CC) -fprofile-arcs -ftest-coverage -DNO_MAIN test.c -o $(TESTNAME)

coverage.html:	test.gcda
	gcovr --html-details coverage.html -e test.c

test.gcda:	test
	./test

coverage.info:
	lcov --capture --directory . --output-file coverage.info

index.html:	coverage.info
	genhtml coverage.info --output-directory .
