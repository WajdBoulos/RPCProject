#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "node.h"
#include "stdbool.h"
#include "RPCImpl.h"
typedef struct Queue* queue;
queue makeQueue();// initiolize a queue 
int getQueueSize(queue q);// return queue size
bool addToQueue(queue q,RPC_Packet *packet);// adds to the end of the queue
void popFromQueue(queue q);//pop in a fifo way
void killQueue(queue to_kill);//free all the queue resorcess
node front(queue q);//return pointer to  the next element in the queue
node back(queue q);// returns pointer to the newest node in the queue;
void popFromQueueByInt(queue q,int fd);//pop in a id way
void removeQuarter(queue q);

#endif