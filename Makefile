CC = gcc
CFLAGS = -c

all: serverapp

serverapp: server.o handler.o 
	$(CC) server.o handler.o -o serverapp


server.o: httpechosrv.c 
	$(CC) $(CFLAGS) httpechosrv.c -o server.o

handler.o: Handler.c 
	$(CC) $(CFLAGS) Handler.c -o handler.o

clean:
	rm serverapp *.o
