CC = g++
CFLAGS = -Wall
STD = -std=c++11
FLAG = -c
FILES = osm.cpp
FILES2 = simpletest.cpp
CLEAN = libosm.a osm.o simpletest.o ex1.tar
TARSRCS = osm.cpp Makefile README

#make

all: libosm.a 

#object files
osm.o: osm.cpp osm.h
	$(CC) $(CFLAGS) $(STD) $(FLAG) $(FILES)

simpletest.o: simpletest.cpp osm.h
	$(CC) $(CFLAGS) $(STD) $(FLAG) $(FILES2)

#Exectubles:
simpletest: simpletest.o osm.o
	$(CC) simpletest.o osm.o -o $@


#Library

libosm.a: osm.o
	ar rcs libosm.a osm.o

tar: libosm.a
	tar -cvf ex1.tar $(TARSRCS)

val: 
	-valgrind --leak-check=full --show-possibly-lost=yes --show-reachable=yes \
	 --undef-value-errors=yes ./simpletest

clean:
	-rm -f $(CLEAN)

.PHONY:clean ,all, tar
