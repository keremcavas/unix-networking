CC=gcc
CFLAGS=-Wall -O2 
OBJECTS=misc.o

all : misc.o
	$(CC) $(CFLAGS) $(OBJECTS) server.c -o Server
	$(CC) $(CFLAGS) $(OBJECTS) client.c -o Client

misc.o : misc.c misc.h
	$(CC) $(CFLAGS) -c misc.c

clean :
	rm -f $(OBJECTS)
	rm -f Client 
	rm -f Server
rebuild :
	make clean
	make all
