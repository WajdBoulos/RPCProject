#include "RPCCommon.h"
#include "queue.h"
#include "RPCHost.h"
#include <time.h>
#define THREAD_NUM 4
/* barrier definitions */
static pthread_mutex_t s_lockWaitJobsDone;
static pthread_cond_t s_condWaitJobsDone;
static int s_numRemainingJobs = 0;

/* server connection */
static int s_serverLen;
static struct sockaddr_in s_serverAddr;
static int s_sockFd;
static uint8_t  s_buf[MAXLINE];
/* requests queue definitions */
static pthread_mutex_t s_lockWaitJobs;
static pthread_cond_t s_condWaitToJobs;
static pthread_cond_t s_condWaitBlockJobs;
static queue s_jobs;
static int s_queueSize = THREAD_NUM;

/* thread definitions */
static pthread_t s_threads[THREAD_NUM + 1];
static uint32_t s_packetId = 0; /* sent packet id number */
/* callback functions definitions */
static RPCFunction (s_funcList[MAX_RPC_FUNCS]);


void _PerformFunction(int funcId, void *args)
{
    s_funcList[funcId](args);
}

/*
 * */
RPC_ReturnStatus _SendPacket(RPC_Packet *packetIn){
    s_serverLen = sizeof(s_serverAddr);
    int n = sendto(s_sockFd, (const char*) packetIn, 4*6+packetIn->argSize, 0,
                   (const struct sockaddr *) &s_serverAddr, s_serverLen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
        return RPC_FAILURE;
    }
    pthread_mutex_lock(&s_lockWaitJobsDone);
    s_numRemainingJobs++;
    pthread_mutex_unlock(&s_lockWaitJobsDone);
    return RPC_SUCCESS;
}

static void* _CallBackHandler()
{
    while(1) {
        pthread_mutex_lock(&s_lockWaitJobs);
        while(getQueueSize(s_jobs) == 0)
        {
            pthread_cond_wait(&s_condWaitToJobs, &s_lockWaitJobs);
        }
        RPC_Packet *rpcPacket1 =  getRequest(front(s_jobs));
        _PerformFunction(rpcPacket1->callBackId, rpcPacket1->argBuf);
        popFromQueue(s_jobs);
        pthread_mutex_lock(&s_lockWaitJobsDone);
        s_numRemainingJobs--;
        pthread_mutex_unlock(&s_lockWaitJobsDone);
        pthread_cond_signal(&s_condWaitJobsDone);
        pthread_mutex_unlock(&s_lockWaitJobs);
        pthread_cond_signal(&s_condWaitBlockJobs);
    }
}


static void* _RecieveHandler()
{
    RPC_Packet * packet;
    while(1) {
        recvfrom(s_sockFd, s_buf, sizeof(s_buf), 0, (struct sockaddr *) NULL, NULL);
        packet = (RPC_Packet*)s_buf;
        pthread_mutex_lock(&s_lockWaitJobs);
        if (getQueueSize(s_jobs) >= s_queueSize) {
            pthread_cond_wait(&s_condWaitBlockJobs, &s_lockWaitJobs);
        }
        addToQueue(s_jobs, packet);
        pthread_mutex_unlock(&s_lockWaitJobs);
        pthread_cond_signal(&s_condWaitToJobs);
    }
}

void _InitThreadPool()
{
    pthread_cond_init(&s_condWaitBlockJobs, NULL);
    pthread_cond_init(&s_condWaitToJobs, NULL);
    if ((pthread_mutex_init(&s_lockWaitJobs, NULL) != 0) ){
        exit(1);
    }
    pthread_cond_init(&s_condWaitJobsDone, NULL);
    if ((pthread_mutex_init(&s_lockWaitJobsDone, NULL) != 0) ){
        exit(1);
    }
    s_jobs = makeQueue();
    for(int i = 0 ; i < THREAD_NUM; i++)
    {
        pthread_create(&(s_threads[i]), NULL, &_CallBackHandler, NULL);
    }
    pthread_create(&(s_threads[THREAD_NUM]), NULL, &_RecieveHandler, NULL);
}

void _Comm_Init(int portno, char* hostname){
    struct hostent* server;
    /* socket: create the socket */
    s_sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s_sockFd < 0) {
        unix_error("Open_clientfd Unix error");
        exit(1);
    }
    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }
    /* build the server's Internet address */
    bzero((char *) &s_serverAddr, sizeof(s_serverAddr));
    s_serverAddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&s_serverAddr.sin_addr.s_addr, server->h_length);
    s_serverAddr.sin_port = htons(portno);
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

RPC_ReturnStatus RPC_Init(RPCFunction* funcArr, const int numFuncs, char* deviceIP, int portNum)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    _Comm_Init(portNum, deviceIP);
    _InitThreadPool();
    return RPC_SUCCESS;
}

RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int inStructSize, int outStructSize)
{
    if(inStructSize > RPC_ARGS_MAX_SIZE || outStructSize > RPC_ARGS_MAX_SIZE)
    {
        return RPC_FAILURE;
    }
    RPC_Packet packet = _CreatePacket(CALL_FUNCTION, funcId, callBackId, args, inStructSize, outStructSize);
    RPC_ReturnStatus rc = _SendPacket(&packet);
    return rc;
}
/*  Function to set a barrier,
 *  The barrier waits to all sent packets to be done.
 * */
void RPC_Barrier()
{
    pthread_mutex_lock(&s_lockWaitJobsDone);
    while(s_numRemainingJobs != 0)
    {
        pthread_cond_wait(&s_condWaitJobsDone, &s_lockWaitJobsDone);
    }
    pthread_mutex_unlock(&s_lockWaitJobsDone);
}
/*  Function to destroy the master and working threads.
 *  It should be called only after the host finished all jobs.
 * */
void RPC_Destroy()
{
    for(int i = 0; i < THREAD_NUM + 1; i++)
    {
        pthread_cancel(s_threads[i]);
    }
}