#include "wrappers.h"
#include "RPCCommon.h"
#include "queue.h"
#include "RPCHost.h"
#include <time.h>
#define THREAD_NUM 4

static pthread_mutex_t lock_wait_jobs_done;
static pthread_cond_t cond_wait_jobs_done;
int num_of_packets = 0;
static int serverlen;
static struct sockaddr_in serveraddr;
static int sockfd;
static pthread_mutex_t lock_wait;
static pthread_cond_t cond_wait;
static pthread_cond_t cond_wait_block;
static uint8_t  buf[MAXLINE];
static queue requests;
static int queue_size = THREAD_NUM;
static pthread_t s_threads[THREAD_NUM + 1];

static void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

static uint32_t s_packetId = 0;

void *_PerformFunction(int funcId, void *args)
{
    return s_funcList[funcId](args);
}

RPC_ReturnStatus _SendPacket(RPC_Packet *packetIn){
    serverlen = sizeof(serveraddr);
    int n = sendto(sockfd, (const char*) packetIn, 4*6+packetIn->argSize, 0, (const struct sockaddr_in*) &serveraddr, serverlen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
        return RPC_FAILURE;
    }
    pthread_mutex_lock(&lock_wait_jobs_done);
    num_of_packets++;
    pthread_mutex_unlock(&lock_wait_jobs_done);
    return RPC_SUCCESS;
}

static void* _CallBackHandler()
{
    while(1) {
        pthread_mutex_lock(&lock_wait);
        while(getQueueSize(requests) == 0)
        {
            pthread_cond_wait(&cond_wait,&lock_wait);
        }
        RPC_Packet *rpcPacket1 =  getRequest(front(requests));
        _PerformFunction(rpcPacket1->callBackId, rpcPacket1->argBuf);
        popFromQueue(requests);
        pthread_mutex_lock(&lock_wait_jobs_done);
        num_of_packets--;
        pthread_mutex_unlock(&lock_wait_jobs_done);
        pthread_cond_signal(&cond_wait_jobs_done);
        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait_block);
    }
}


static void* _RecieveHandler()
{
    RPC_Packet * packet;
    while(1) {
        Recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) NULL, NULL);
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

void _InitThreadPool()
{
    pthread_cond_init(&cond_wait_block,NULL);
    pthread_cond_init(&cond_wait,NULL);
    if ((pthread_mutex_init(&lock_wait, NULL) != 0) ){
        exit(1);
    }
    pthread_cond_init(&cond_wait_jobs_done, NULL);
    if ((pthread_mutex_init(&lock_wait_jobs_done, NULL) != 0) ){
        exit(1);
    }
    requests = makeQueue();
    for(int i = 0 ; i < THREAD_NUM; i++)
    {
        pthread_create(&(s_threads[i]), NULL, &_CallBackHandler, NULL);
    }
    pthread_create(&(s_threads[THREAD_NUM]), NULL, &_RecieveHandler, NULL);
}

void _Comm_Init(int portno, char* hostname){
    struct hostent* server;
    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
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
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
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

RPC_ReturnStatus RPC_Init(void* *funcArr(void *), const int numFuncs, char* deviceIP, int portNum)
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

RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int argSize, int retSize, void **callBackResPtr)
{
    RPC_Packet packet = _CreatePacket(CALL_FUNCTION, funcId, callBackId, args, argSize, retSize);
    RPC_ReturnStatus rc = _SendPacket(&packet);
    return rc;
}


void RPC_Barrier() {
    pthread_mutex_lock(&lock_wait_jobs_done);
    while(num_of_packets != 0) {
        pthread_cond_wait(&cond_wait_jobs_done,&lock_wait_jobs_done);
    }
    pthread_mutex_unlock(&lock_wait_jobs_done);
}
