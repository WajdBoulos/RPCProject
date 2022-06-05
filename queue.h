#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "node.h"
#include "stdbool.h"
#include "RPCCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Queue* queue;
queue makeQueue();// initialize a queue
int getQueueSize(queue q);// return queue size
bool addToQueue(queue q,RPC_Packet *packet);// adds to the end of the queue
void popFromQueue(queue q);//pop in a fifo way
void killQueue(queue to_kill);//free all the queue resorcess
node front(queue q);//return pointer to  the next element in the queue
node back(queue q);// returns pointer to the newest node in the queue;
void popFromQueueByInt(queue q,int packetId);//pop in an id.

#ifdef __cplusplus
}
#endif

#endif