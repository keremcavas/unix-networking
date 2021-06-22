CC=gcc
CFLAGS=-Wall -O2 
OBJECTS=dns.o
OUTPUT=Resolver

all : dns.o
	$(CC) $(CFLAGS) $(OBJECTS) main.c -o $(OUTPUT)

dns.o : dns.c dns.h
	$(CC) $(CFLAGS) -c dns.c

clean :
	rm -f $(OBJECTS)
	rm -f $(OUTPUT)

rebuild :
	make clean
	make all
