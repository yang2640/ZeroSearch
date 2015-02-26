# Makefile for imagelib

MACHTYPE=$(shell uname -m)

GCC			= x86_64-nacl-gcc

CC=x86_64-nacl-gcc
OPTFLAGS=-g2
# OPTFLAGS=-O3
OTHERFLAGS=-Wall

IMAGELIB_OBJS= qsort.o util.o

CFLAGS=$(OTHERFLAGS) $(OPTFLAGS)

IMAGELIB=libimage.a

%.$(P).o : %.c
	$(CC) -c -o $@ $(CFLAGS) $<

all: $(IMAGELIB)

$(IMAGELIB): $(IMAGELIB_OBJS)
	x86_64-nacl-ar rusv $(IMAGELIB) $(IMAGELIB_OBJS)
	x86_64-nacl-ranlib $(IMAGELIB)
	cp $(IMAGELIB) ..

clean:
	rm -f *.o *~ $(IMAGELIB)
