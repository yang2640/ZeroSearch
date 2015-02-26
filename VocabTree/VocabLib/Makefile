# Makefile for VocabLib

MACHTYPE=$(shell uname -m)

GCC			= x86_64-nacl-g++

CC=x86_64-nacl-g++
# OPTFLAGS=-g2
OPTFLAGS=-O3 -ffast-math -Wall -mfpmath=sse -msse2 -funroll-loops
OTHERFLAGS=-Wall

INCLUDE_PATH=-I../lib/ann_1.1/include/ANN -I../lib/ann_1.1_char/include/ANN \
	-I../lib/imagelib 

OBJS=keys2.o kmeans.o kmeans_kd.o VocabTreeBuild.o VocabTreeIO.o \
	VocabTreeUtil.o VocabTree.o VocabFlatNode.o

CPPFLAGS=$(INCLUDE_PATH) $(OTHERFLAGS) $(OPTFLAGS)

LIB=libvocab.a

all: $(LIB)

$(LIB): $(OBJS)
	x86_64-nacl-ar ruvs $(LIB) $(OBJS)
	x86_64-nacl-ranlib $(LIB)
	cp $(LIB) ..

%.o: %.cpp
	$(GCC) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~ $(LIB)
	rm ../$(LIB)
