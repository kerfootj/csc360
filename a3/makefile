#
# "makefile" for the CPU scheduler simulation.
#

CC=gcc
CFLAGS=-c -Wall -g -DDEBUG

all: rrsim fcfssim sjfsim simgen testlist

simgen: simgen.o
	$(CC) simgen.o -o simgen

simgen.o: simgen.c
	$(CC) $(CFLAGS) simgen.c

rrsim: rrsim.o linkedlist.o
	$(CC) rrsim.o linkedlist.o -o rrsim

rrsim.o: rrsim.c linkedlist.h
	$(CC) $(CFLAGS) rrsim.c

fcfssim: fcfssim.o linkedlist.o
	$(CC) fcfssim.o linkedlist.o -o fcfssim

fcfssim.o: fcfssim.c linkedlist.h
	$(CC) $(CFLAGS) fcfssim.c

sjfsim: sjfsim.o linkedlist.o
	$(CC) sjfsim.o linkedlist.o -o sjfsim

sjfsim.o: sjfsim.c linkedlist.h
	$(CC) $(CFLAGS) sjfsim.c

testlist: testlist.o linkedlist.o
	$(CC) testlist.o linkedlist.o -o testlist

testlist.o: testlist.c linkedlist.h
	$(CC) $(CFLAGS) testlist.c

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) linkedlist.c

clean:
	rm -rf *.o rrsim fcfssim sjfsim simgen testlist
