#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = RPCDevice.o RPCDeviceExample.o node.o queue.o
TARGET = RPCDevice

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: RPCDevice

RPCDevice: RPCDeviceExample.o RPCDevice.o RPCCommon.o queue.o node.o
	$(CC) $(CFLAGS) -o RPCDevice RPCDeviceExample.o RPCDevice.o RPCCommon.o queue.o node.o $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
.node.o: node.o
	$(CC) $(CFLAGS) -o node node.o
.queue.o: node.o queue.o
		$(CC) $(CFLAGS) -o queue queue.o node.o
clean:
	-rm -f $(OBJS) RPCDevice