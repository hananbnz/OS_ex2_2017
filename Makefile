CC = g++
CFLAGS = -Wall
STD = -std=c++11
FLAG = -c
FILES = uthread.cpp uthreads.h Thread.cpp Thread.h
FILES2 = Thread.cpp
CLEAN = uthread.a uthread.o Thread.o ex2.tar
TARSRCS = osm.cpp Makefile README

#make

all: libuthread.a 

#object files
uthread.o: uthread.cpp uthreads.h Thread.cpp Thread.h
	$(CC) $(CFLAGS) $(STD) $(FLAG) $(FILES)

Thread.o: Thread.cpp Thread.h
	$(CC) $(CFLAGS) $(STD) $(FLAG) $(FILES2)

#Exectubles:
simpletest: simpletest.o osm.o
	$(CC) simpletest.o osm.o -o $@


#Library

libuthread.a: uthread.o
	ar rcs libuthread.a uthread.o

tar: libuthread.a
	tar -cvf ex2.tar $(TARSRCS)

val: 
	-valgrind --leak-check=full --show-possibly-lost=yes --show-reachable=yes \
	 --undef-value-errors=yes ./simpletest

clean:
	-rm -f $(CLEAN)

.PHONY:clean ,all, tar
