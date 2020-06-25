CC=gcc
CFLAGS=-I.

lib: header.c
	$(CC) -o header.o -c header.c $(CFLAGS)
	ar rcs libheader.a header.o

server: server.c libhead.a
	$(CC) -o server.o -c server.c $(CFLAGS)
	$(CC) -o server server.o -L -lhead -pthread  $(CFLAGS)

client: client.c libhead.a
	$(CC) -o client.o -c client.c $(CFLAGS)
	$(CC) -o client client.o -L -lhead $(CFLAGS)

clean:
	rm client server client.o server.o header.o libhead.a
