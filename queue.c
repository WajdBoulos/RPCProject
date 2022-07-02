#include "queue.h"
#include "math.h"
#include "RPCCommon.h"

struct Queue
{
    node head;
    node end;
    int size;
};

queue makeQueue()
{
    queue newQueue = malloc(sizeof(*newQueue));
    newQueue->size = 0;
    struct timeval arrival;
    newQueue->head = makeNode(NULL,-1);
    newQueue->end = makeNode(NULL,-2);
    specialConnect(newQueue->head,newQueue->end);
    return newQueue;
}

int getQueueSize(queue q)
{
    return q->size;
}
bool addToQueue(queue q,RPC_Packet *request)
{
    node newNode= makeNode(request,0);
    
    connectNode(q->end,newNode);
    q->size = q->size+1;
    return 1;
}
void popFromQueue(queue q)
{
    node newNode = getNext(q->head);
    disconnectNode(newNode);
    q->size = q->size-1;
}

void killQueue(queue toKill)
{
    deleteall(toKill->head);
    free(toKill);
}

node front(queue q)
{
    // printf("in head\n");
    return getNext(q->head);
}

node back(queue q)
{
    return getPrevious(q->end);
}

void popFromQueueByInt(queue q,int packetId)
{
    node temp = getNext(q->head);

    while(getPacket(temp)->packetId != packetId)
    {
        temp = getNext(temp);
    }
    disconnectNode(temp);
    q->size = q->size -1;
}
