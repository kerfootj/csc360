#
# "makefile" for the disk-image file-system assignment
#

CC=gcc
CFLAGS=-c -Wall -g -DDEBUG

all: statuvfs lsuvfs catuvfs storuvfs

statuvfs: statuvfs.o
	$(CC) statuvfs.o -o statuvfs

statuvfs.o: statuvfs.c disk.h
	$(CC) $(CFLAGS) statuvfs.c

lsuvfs: lsuvfs.o
	$(CC) lsuvfs.o -o lsuvfs

lsuvfs.o: lsuvfs.c disk.h
	$(CC) $(CFLAGS) lsuvfs.c

catuvfs: catuvfs.o
	$(CC) catuvfs.o -o catuvfs

catuvfs.o: catuvfs.c disk.h
	$(CC) $(CFLAGS) catuvfs.c

storuvfs: storuvfs.o
	$(CC) storuvfs.o -o storuvfs

storuvfs.o: storuvfs.c disk.h
	$(CC) $(CFLAGS) storuvfs.c

clean:
	rm -rf *.o statuvfs lsuvfs catuvfs storuvfs
