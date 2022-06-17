#include "RPCCommon.h"
#include "queue.h"
#include "RPCHost.h"
#include <time.h>
#include <unistd.h>

#define THREAD_NUM 4
#define MAX_WINDOW_SIZE 100
#define WINDOW_UNLOCK_SIZE (MAX_WINDOW_SIZE/2)
#define SEND_PACKET_TIMEOUT 10

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
static int s_numFucs = 0;

static void _PerformFunction(int funcId, void *args)
{
    if(funcId > s_numFucs - 1)
    {
        unix_error("Accessing an invalid function. Make sure you properly pass number of functions in the init.");
    }
    s_funcList[funcId](args);
}

/*
 * */
static RPC_ReturnStatus _SendPacket(RPC_Packet *packetIn)
{
    s_serverLen = sizeof(s_serverAddr);
    int n = sendto(s_sockFd, (const char*) packetIn, sizeof(int32_t)*5+packetIn->inStructSize, 0,
                   (const struct sockaddr *) &s_serverAddr, s_serverLen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
        return RPC_FAILURE;
    }

    pthread_mutex_lock(&s_lockWaitJobsDone);
    s_numRemainingJobs++;
    int remainJobs = s_numRemainingJobs;
    pthread_mutex_unlock(&s_lockWaitJobsDone);

    struct timeval t0, t1, dt;
    gettimeofday(&t0, NULL);
    if(remainJobs > MAX_WINDOW_SIZE)
    {
        while (1)
        {
            pthread_mutex_lock(&s_lockWaitJobsDone);
            if (s_numRemainingJobs < WINDOW_UNLOCK_SIZE)
            {
                pthread_mutex_unlock(&s_lockWaitJobsDone);
                break;
            }
            pthread_mutex_unlock(&s_lockWaitJobsDone);
            gettimeofday(&t1, NULL);
            timersub(&t1, &t0, &dt);
            double totTime = (1e+6 *  (double)dt.tv_sec +  (double)dt.tv_usec)/(double)1;
            if(totTime > 1e+6 * SEND_PACKET_TIMEOUT) // return error if passes SEND_PACKET_TIMEOUT seonds and WINDOW_UNLOCK_SIZE jobs havent been finished
            {
                return RPC_FAILURE;
            }
        }
    }
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
    return NULL;
}


static void* _RecieveHandler()
{
    RPC_Packet * packet;
    while(1)
    {
        recvfrom(s_sockFd, s_buf, sizeof(s_buf), 0, (struct sockaddr *) NULL, NULL);
        packet = (RPC_Packet*)s_buf;
        pthread_mutex_lock(&s_lockWaitJobs);
        if (getQueueSize(s_jobs) >= s_queueSize)
        {
            pthread_cond_wait(&s_condWaitBlockJobs, &s_lockWaitJobs);
        }
        addToQueue(s_jobs, packet);
        pthread_mutex_unlock(&s_lockWaitJobs);
        pthread_cond_signal(&s_condWaitToJobs);
    }
    return NULL;
}

static RPC_ReturnStatus _InitThreadPool()
{
    pthread_cond_init(&s_condWaitBlockJobs, NULL);
    pthread_cond_init(&s_condWaitToJobs, NULL);
    if ((pthread_mutex_init(&s_lockWaitJobs, NULL) != 0) )
    {
        return RPC_FAILURE;
    }
    pthread_cond_init(&s_condWaitJobsDone, NULL);
    if ((pthread_mutex_init(&s_lockWaitJobsDone, NULL) != 0) )
    {
        return RPC_FAILURE;
    }
    s_jobs = makeQueue();
    for(int i = 0 ; i < THREAD_NUM; i++)
    {
        pthread_create(&(s_threads[i]), NULL, &_CallBackHandler, NULL);
    }
    pthread_create(&(s_threads[THREAD_NUM]), NULL, &_RecieveHandler, NULL);
    return RPC_SUCCESS;
}

static RPC_ReturnStatus _Comm_Init(int portno, char* hostname)
{
    struct hostent* server;
    /* socket: create the socket */
    s_sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s_sockFd < 0) {
        unix_error("Open_clientfd Unix error");
        return RPC_FAILURE;
    }
    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        return RPC_FAILURE;
    }
    /* build the server's Internet address */
    bzero((char *) &s_serverAddr, sizeof(s_serverAddr));
    s_serverAddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&s_serverAddr.sin_addr.s_addr, server->h_length);
    s_serverAddr.sin_port = htons(portno);
    return RPC_SUCCESS;
}

static inline RPC_Packet _CreatePacket(int funcId, int callBackId, void *args, int argSize, int retSize) {
    RPC_Packet packet;
    packet.funcId = funcId;
    packet.callBackId = callBackId;
    packet.inStructSize = argSize;
    packet.outStructSize = retSize;
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

    s_numFucs = numFuncs;
    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    RPC_ReturnStatus rc = _Comm_Init(portNum, deviceIP);
    rc |= _InitThreadPool();
    return rc;
}

RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int inStructSize, int outStructSize)
{
    if(inStructSize > RPC_ARGS_MAX_SIZE || outStructSize > RPC_ARGS_MAX_SIZE)
    {
        return RPC_FAILURE;
    }
    RPC_Packet packet = _CreatePacket(funcId, callBackId, args, inStructSize, outStructSize);
    RPC_ReturnStatus rc = _SendPacket(&packet);
    return rc;
}

RPC_ReturnStatus RPC_Barrier(double timeoutSecs)
{
    struct timeval t0, t1, dt;
    gettimeofday(&t0, NULL);

    while(1)
    {
        pthread_mutex_lock(&s_lockWaitJobsDone);
        if(s_numRemainingJobs == 0)
        {
            pthread_mutex_unlock(&s_lockWaitJobsDone);
            break;
        }
        pthread_mutex_unlock(&s_lockWaitJobsDone);
        gettimeofday(&t1, NULL);
        timersub(&t1, &t0, &dt);
        double totTime = (1e+6 *  (double)dt.tv_sec +  (double)dt.tv_usec)/(double)1;
        if(totTime > 1e+6 * timeoutSecs)
        {
            return RPC_FAILURE;
        }
    }
    return RPC_SUCCESS;
}

void RPC_Destroy()
{
    for(int i = 0; i < THREAD_NUM + 1; i++)
    {
        pthread_cancel(s_threads[i]);
    }
}