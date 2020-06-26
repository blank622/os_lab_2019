CC=gcc
CFLAGS=-I.

all : server client

server : server.o 
	$(CC) -o server -lpthread server.o -L. -lmult_modulo -pthread

client : client.o 
	$(CC) -o client client.o -L. -lmult_modulo -pthread

server.o :
	$(CC) -o server.o -c -std=c99 -L. -lmult_modulo server.c $(CFLAGS) -pthread

client.o : 
	$(CC) -o client.o -c -std=c99 -L. -lmult_modulo client.c $(CFLAGS) -pthread

clean : 
	rm client.o client server.o server 