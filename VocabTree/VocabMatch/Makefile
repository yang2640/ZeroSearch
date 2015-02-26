# Makefile for VocabMatch

MACHTYPE=$(shell uname -m)

GCC			= x86_64-nacl-g++

CC=x86_64-nacl-gcc
# OPTFLAGS=-g2
OPTFLAGS=-O3 
OTHERFLAGS=-Wall

INCLUDE_PATH=-I../lib/ann_1.1/include/ANN -I../lib/ann_1.1_char/include/ANN \
	-I../lib/imagelib -I../VocabLib -I../lib/zlib/include
LIB_PATH=-L../lib -L../VocabLib

OBJS=VocabMatch.o
OBJS_DESC=VocabMatch_desc.o

LIBS=-lvocab -lANN -lANN_char -limage -lz

CPPFLAGS=$(INCLUDE_PATH) $(LIB_PATH) $(OTHERFLAGS) $(OPTFLAGS)

BIN=VocabMatch.nexe
BIN_DESC=VocabMatch_desc

all: $(BIN) $(BIN_DESC) VocabMatchScript VocabMatchScript_desc

$(BIN): $(OBJS)
	$(GCC) -o $(CPPFLAGS) -o $(BIN) $(OBJS) $(LIBS)

$(BIN_DESC): $(OBJS_DESC)
	$(GCC) -o $(CPPFLAGS) -o $@ $^ $(LIBS)

VocabMatchScript: VocabMatchScript.o
	$(GCC)  $(CPPFLAGS) -o $@ $^ $(LIBS)

VocabMatchScript_desc: VocabMatchScript_desc.o
	$(GCC)  $(CPPFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(GCC) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -f *.o *~ $(BIN_DESC) $(BIN) VocabMatchScript VocabMatchScript_desc
