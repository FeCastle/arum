CC	= /usr/bin/g++
CFLAGS	= -Wall
CXXFLAGS=-O3

all : ./lib/libbinpro.so

./lib/libbinpro.so : binpro.o
	$(CC) $(CXXFLAGS) -shared -fPIC -Wl,-rpath=./lib binpro.o -L./lib -ldyninstAPI -ldwarf -linstructionAPI -lsymtabAPI -lcommon -lparseAPI -o ./lib/libbinpro.so

binpro.o: binpro.C
	$(CC) -g -fPIC -c $(CFLAGS) -I./include binpro.C

clean:
	@rm -f ./lib/libbinpro.so binpro.o

