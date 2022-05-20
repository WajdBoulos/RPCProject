#include "node.h"
struct Node
{
    RPC_Packet *packet;
    node next;
    node previous;
    int head;
};
node makeNode(RPC_Packet *packet, int index)
{
    node new_node = malloc(sizeof(*new_node));
    new_node->packet = packet;
    new_node->next = NULL;
    new_node->previous = NULL;
    new_node->head = index;
    return new_node;

}
void specialConnect(node start,node end)
{
        start->next = end;
        end->previous = start;
}
void connectNode(node end,node connecting)
{
    node tmp_node = end->previous;
    tmp_node->next = connecting;
    connecting ->previous = tmp_node;
    connecting->next = end;
    end->previous = connecting;
}
void disconnectNode(node to_disconnect)
{
   node tmp_node = to_disconnect->next;
   node tmp_node2 = to_disconnect->previous;
   tmp_node->previous = tmp_node2;
   tmp_node2->next = tmp_node;
   destroyNode(to_disconnect);
}
void destroyNode(node to_destroy)
{
    to_destroy->next = NULL;
    to_destroy->previous = NULL;
    free(to_destroy);
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
    return nd->packet;
}
