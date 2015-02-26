# Makefile for VocabBuildDB

MACHTYPE=$(shell uname -m)

GCC			= x86_64-nacl-g++

CC=x86_64-nacl-gcc
# OPTFLAGS=-g2
OPTFLAGS=-O3 
OTHERFLAGS=-Wall

INCLUDE_PATH=-I../lib/ann_1.1/include/ANN -I../lib/ann_1.1_char/include/ANN \
	-I../lib/imagelib -I../VocabLib -I../lib/zlib/include
LIB_PATH=-L../lib -L../VocabLib -L../lib/zlib/lib

OBJS=VocabBuildDB.o

LIBS=-lvocab -lANN -lANN_char -limage -lz

CPPFLAGS=$(INCLUDE_PATH) $(LIB_PATH) $(OTHERFLAGS) $(OPTFLAGS)

BIN=VocabBuildDB

all: $(BIN)

$(BIN): $(OBJS)
	$(GCC)  $(CPPFLAGS) -o $(BIN) $(OBJS) $(LIBS)

$(OBJS): VocabBuildDB.cpp
	 $(GCC)  $(CPPFLAGS) -c -o $(OBJS) VocabBuildDB.cpp

clean:
	rm -f *.o *~ $(LIB)
