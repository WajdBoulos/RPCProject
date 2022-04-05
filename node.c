#include "node.h"
struct Node
{
    int request;
    node next;
    node previous;
};
node makeNode(int request)
{
    node new_node = malloc(sizeof(*new_node));
    new_node->request = request;
    new_node->next = NULL;
    new_node->previous = NULL;
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
    if(nd->next->request == -2)
    {
        return NULL;
    }
    return nd->next;
}
node getPrevious(node nd)
{
    if(nd->previous->request == -1)
    {
        return NULL;
    }
    return nd->previous;
}
int getRequest(node nd)
{
    return nd->request;
}
