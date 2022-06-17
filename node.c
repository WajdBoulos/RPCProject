#include <string.h>
#include "node.h"

struct Node
{
    RPC_Packet packet;
    node next;
    node previous;
    int head;
};

node makeNode(RPC_Packet *packet, int index)
{
    node newNode = malloc(sizeof(*newNode));
  //  newNode->packet = malloc(sizeof(*packet));
    if(packet)
    memcpy(&newNode->packet, packet, sizeof(RPC_Packet));
    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->head = index;
    return newNode;
}

void specialConnect(node start,node end)
{
        start->next = end;
        end->previous = start;
}

void connectNode(node end,node connecting)
{
    node tmpNode = end->previous;
    tmpNode->next = connecting;
    connecting ->previous = tmpNode;
    connecting->next = end;
    end->previous = connecting;
}

void disconnectNode(node to_disconnect)
{
   node tmpNode = to_disconnect->next;
   node tmpNode2 = to_disconnect->previous;
   tmpNode->previous = tmpNode2;
   tmpNode2->next = tmpNode;
   destroyNode(to_disconnect);
}

void destroyNode(node toDestroy)
{
    toDestroy->next = NULL;
    toDestroy->previous = NULL;
    free(toDestroy);
}

void deleteall(node start)
{
    node tmp = start->next;
    destroyNode(start);
    while(tmp->next != NULL)
    {
        tmp = tmp->next;
        destroyNode(tmp->previous);
    }
    destroyNode(tmp);
}

node getNext(node nd)
{
    if(nd->next->head == -2)
    {
        return NULL;
    }
    return nd->next;
}

node getPrevious(node nd)
{
    if(nd->previous->head == -1)
    {
        return NULL;
    }
    return nd->previous;
}

RPC_Packet *getRequest(node nd)
{
    return &nd->packet;
}
