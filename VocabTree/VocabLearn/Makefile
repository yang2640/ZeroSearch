# Makefile for VocabLearn

MACHTYPE=$(shell uname -m)

GCC			= x86_64-nacl-g++

CC=x86_64-nacl-gcc
# OPTFLAGS=-g2
OPTFLAGS=-O3 -ffast-math -Wall -mfpmath=sse -msse2 -funroll-loops
OTHERFLAGS=-Wall

INCLUDE_PATH=-I../lib/ann_1.1/include/ANN -I../lib/ann_1.1_char/include/ANN \
	-I../lib/imagelib -I../VocabLib -I../lib/zlib/include
LIB_PATH=-L../lib -L../VocabLib -L../lib/zlib/lib

OBJS=VocabLearn.o

LIBS=-lvocab -lANN -lANN_char -limage -lz

CPPFLAGS=$(INCLUDE_PATH) $(LIB_PATH) $(OTHERFLAGS) $(OPTFLAGS)

BIN=VocabLearn

all: $(BIN)

$(BIN): $(OBJS)
	$(GCC) $(CPPFLAGS) -o $(BIN) $(OBJS) $(LIBS)

%.o: %.cpp
	$(GCC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~ $(BIN)
