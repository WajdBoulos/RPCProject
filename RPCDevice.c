#include "RPCCommon.h"
#include "stdbool.h"
#include "queue.h"
#include "RPCDevice.h"

#define THREAD_NUM 4


static void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

static uint32_t s_packetId = 0;

/* threads*/
static pthread_mutex_t lock_wait;
static pthread_cond_t cond_wait;
static pthread_cond_t cond_wait_block;
static uint8_t  buf[MAXLINE];

static queue requests;
static int queue_size = THREAD_NUM;
static int listenfd, clientlen;
static struct sockaddr_in clientaddr;
static pthread_t s_threads[THREAD_NUM + 1];

void *_PerformFunction(int funcId, void *args)
{
    return s_funcList[funcId](args);
}

int _SendPacket(RPC_Packet *packet){
    int n = sendto(listenfd, (const char*) packet, 4*6+packet->retSize, 0, (SA *) &clientaddr, clientlen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
    }
    return 1;
}

static void* _CallBackHandler()
{
    while(1) {
        pthread_mutex_lock(&lock_wait);
        while(getQueueSize(requests) == 0)
        {
            pthread_cond_wait(&cond_wait,&lock_wait);
        }
        RPC_Packet rpcPacket1;
        memcpy(&rpcPacket1, getRequest(front(requests)), sizeof (RPC_Packet));
        popFromQueue(requests);
        pthread_mutex_unlock(&lock_wait);
        _PerformFunction(rpcPacket1.funcId, rpcPacket1.argBuf);
        _SendPacket(&rpcPacket1);
        pthread_cond_signal(&cond_wait_block);
    }

}

static void* _RecieveHandler()
{
    RPC_Packet * packet;
    bool alive = 1;
    while(alive) {
        recvfrom(listenfd, buf, MAXBUF,0, (SA *)&clientaddr, (socklen_t *) &clientlen);
        packet = (RPC_Packet*)buf;
        pthread_mutex_lock(&lock_wait);
        if (getQueueSize(requests)  >= queue_size) {
            pthread_cond_wait(&cond_wait_block,&lock_wait);
        }
        addToQueue(requests, packet);
        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait);
    }
}

static void _InitThreadPool()
{
    pthread_cond_init(&cond_wait_block,NULL);

    pthread_cond_init(&cond_wait,NULL);
    if ((pthread_mutex_init(&lock_wait, NULL) != 0) ){
        exit(1);
    }
    requests = makeQueue();
    pthread_t* threads = s_threads;
    for(int i = 0 ; i < THREAD_NUM; i++)
    {
        pthread_create(&(threads[i]), NULL, &_CallBackHandler, NULL);
    }

    pthread_create(&(threads[THREAD_NUM]), NULL, &_RecieveHandler, NULL);
}

static void _Comm_Init(int port){
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(listenfd < 0){
        unix_error("Open_listenfd Unix error");
        exit(1);
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(listenfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0){
        unix_error("Open_listenfd Unix error");
        exit(1);
    }
    clientlen = sizeof(clientaddr);
}

static inline RPC_Packet _CreatePacket(int command, int funcId, int callBackId, void *args, int argSize, int retSize) {
    RPC_Packet packet;
    packet.funcId = funcId;
    packet.cmd = command;
    packet.callBackId = callBackId;
    packet.argSize = argSize;
    packet.retSize = retSize;
    packet.packetId = s_packetId++;
    memcpy(packet.argBuf, args, argSize);
    return packet;
}

RPC_ReturnStatus RPC_Init(void* *funcArr(void *), const int numFuncs, int portNum)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    _Comm_Init(portNum);
    _InitThreadPool();
    return RPC_SUCCESS;
}
