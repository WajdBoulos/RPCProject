#include "queue.h"
#include "math.h"
#include "wrappers.h"
struct Queue
{
    node head;
    node end;
    int size;
};
queue makeQueue()
{
      queue new_queue = malloc(sizeof(*new_queue));
      new_queue->size = 0;
      struct timeval arrival;
     new_queue->head = makeNode(-1);
     new_queue->end = makeNode(-2);
     specialConnect(new_queue->head,new_queue->end);
     return new_queue;
}

int getQueueSize(queue q)
{
    return q->size;
}
bool addToQueue(queue q,int request)
{
    node new_nod = makeNode(request);
    
    connectNode(q->end,new_nod);
    q->size = q->size+1;
    return 1;
}
void popFromQueue(queue q)
{
    node new_node = getNext(q->head);
    disconnectNode(new_node);
    q->size = q->size-1;
}
void killQueue(queue to_kill)
{
    deleteall(to_kill->head);
    free(to_kill);
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
void popFromQueueByInt(queue q,int fd)
{
    node temp = getNext(q->head);
    while(getRequest(temp) != fd)
    {
        temp = getNext(temp);
    }
    disconnectNode(temp);
    q->size = q->size -1;
}
void removeQuarter(queue q)
{
    int size = q->size;
    while(q->size > ceil(size*3/4))
    {
        srand(time(0));
      int index_to_remove = rand()%(q->size);
      node tmp = getNext(q->head);
      while(index_to_remove > 0 )
      {
          tmp = getNext(tmp);
          index_to_remove--;
      }
      Close(getRequest(tmp));
      disconnectNode(tmp);
      q->size = q->size-1;
    }
}

