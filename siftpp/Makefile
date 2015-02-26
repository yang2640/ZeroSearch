CC=x86_64-nacl-g++
CFLAGS=-I. -Wall -g -O3 -DNDEBUG -DVL_LOWE_STRICT  -DVL_USEFASTMATH
all:sift.nexe

sift.nexe:sift.o sift-driver.o
	$(CC) $(CFLAGS) $^ -o $@

sift.o:sift.cpp
	$(CC) $(CFLAGS) $< -c -o $@

sift-driver.o:sift-driver.cpp
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm *.o
	rm sift.nexe
    
