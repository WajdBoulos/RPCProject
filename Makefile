#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o segel.o client.o node.o queue.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server client
	-mkdir -p public
	-cp public

server: server.o wrappers.o queue.o node.o
	$(CC) $(CFLAGS) -o server server.o wrappers.o queue.o node.o $(LIBS)

client: client.o wrappers.o
	$(CC) $(CFLAGS) -o client client.o wrappers.o
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
.node.o: node.o
	$(CC) $(CFLAGS) -o node node.o
.queue.o: node.o queue.o
		$(CC) $(CFLAGS) -o queue queue.o node.o
clean:
	-rm -f $(OBJS) server client
	-rm -rf public
