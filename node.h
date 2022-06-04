#ifndef __NODE_H__
#define __NODE_H__
#include "stdlib.h"
#include "sys/time.h"
#include "time.h"
#include "RPCCommon.h"
typedef struct Node* node;
node makeNode(RPC_Packet *, int);// initiolize new node
void specialConnect(node start,node end);// to connect the end node to the start node 
void connectNode(node start,node connecting);// connects between 2 nodes 
void disconnectNode(node to_disconnect);//disconnect a node 
void destroyNode(node to_destroy);// free node resorcess 
void deleteall(node start);// kill all connected nodes 
node getNext(node nd);// return the next value of a node 
node getPrevious(node nd);//return the previous value of a node 
RPC_Packet * getRequest(node nd);//get request from node
#endif