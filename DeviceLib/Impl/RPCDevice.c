#include "../../Common/RPCCommon.h"
#include "stdbool.h"
#include "../../Common/queue.h"
#include "../RPCDevice.h"

#define THREAD_NUM 4

static uint32_t s_packetId = 0;

/* threads*/
static pthread_mutex_t s_lockWait;
static pthread_cond_t s_condWait;
static pthread_cond_t s_condWaitBlock;
static uint8_t  s_buf[MAXLINE];

static queue s_requests;
static int s_queueSize = THREAD_NUM;
static int s_listenFd, s_clientLen;
static struct sockaddr_in s_clientAddr;

static pthread_t s_threads[THREAD_NUM + 1];

static RPCFunction (s_funcList[MAX_RPC_FUNCS]);
static int s_numFucs = 0;

static void _PerformFunction(int funcId, void *args)
{
    if(funcId > s_numFucs - 1)
    {
        unix_error("Accessing an invalid function. Make sure you properly pass number of functions in the init.");
    }
    s_funcList[funcId](args);
}


static int _SendPacket(RPC_Packet *packet)
{
    int n = sendto(s_listenFd, (const char*) packet, sizeof(int32_t) * 5 + packet->outStructSize, 0, (SA *) &s_clientAddr, s_clientLen);
    if (n < 0)
    {
        unix_error("Open_clientfd Unix error");
    }
    return 1;
}

static void* _WorkerHandler()
{
    while(1)
    {
        pthread_mutex_lock(&s_lockWait);
        while(getQueueSize(s_requests) == 0)
        {
            pthread_cond_wait(&s_condWait, &s_lockWait);
        }
        RPC_Packet rpcPacket1;
        memcpy(&rpcPacket1, getPacket(front(s_requests)), sizeof (RPC_Packet));
        popFromQueue(s_requests);
        pthread_mutex_unlock(&s_lockWait);
        _PerformFunction(rpcPacket1.funcId, rpcPacket1.argBuf);
        _SendPacket(&rpcPacket1);
        pthread_cond_signal(&s_condWaitBlock);
    }
    return NULL;
}

static void* _MasterHandler()
{
    RPC_Packet * packet;
    while(1)
    {
        recvfrom(s_listenFd, s_buf, MAXBUF, 0, (SA *)&s_clientAddr, (socklen_t *) &s_clientLen);
        packet = (RPC_Packet*)s_buf;
        pthread_mutex_lock(&s_lockWait);
        if (getQueueSize(s_requests) >= s_queueSize)
        {
            pthread_cond_wait(&s_condWaitBlock, &s_lockWait);
        }
        s_packetId++;
        addToQueue(s_requests, packet);
        pthread_mutex_unlock(&s_lockWait);
        pthread_cond_signal(&s_condWait);
    }
    return NULL;
}

static void _InitThreadPool()
{
    pthread_cond_init(&s_condWaitBlock, NULL);

    pthread_cond_init(&s_condWait, NULL);
    if ((pthread_mutex_init(&s_lockWait, NULL) != 0) )
    {
        exit(1);
    }
    s_requests = makeQueue();
    pthread_t* threads = s_threads;
    for(int i = 0 ; i < THREAD_NUM; i++)
    {
        pthread_create(&(threads[i]), NULL, &_WorkerHandler, NULL);
    }

    pthread_create(&(threads[THREAD_NUM]), NULL, &_MasterHandler, NULL);
}

static void _Comm_Init(int port)
{
    struct sockaddr_in serveraddr;
    s_listenFd = socket(AF_INET, SOCK_DGRAM, 0);
    if(s_listenFd < 0)
    {
        unix_error("Open_listenfd Unix error");
        exit(1);
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    if (bind(s_listenFd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
    {
        unix_error("Open_listenfd Unix error");
        exit(1);
    }
    s_clientLen = sizeof(s_clientAddr);
}

RPC_ReturnStatus RPC_Init(RPCFunction* funcArr, const int numFuncs, int portNum)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    s_numFucs = numFuncs;
    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    _Comm_Init(portNum);
    _InitThreadPool();
    return RPC_SUCCESS;
}
