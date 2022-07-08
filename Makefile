#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = RPCHost.o RPCHostExample.o node.o queue.o
TARGET = RPCHost

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: RPCHost

RPCDevice: RPCHostExample.o RPCHost.o queue.o node.o
	$(CC) $(CFLAGS) -o RPCHost RPCHostExample.o RPCHost.o queue.o node.o $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
.node.o: node.o
	$(CC) $(CFLAGS) -o node node.o
.queue.o: node.o queue.o
		$(CC) $(CFLAGS) -o queue queue.o node.o
clean:
	-rm -f $(OBJS) RPCDevice
