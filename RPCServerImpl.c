#include "wrappers.h"
#include "RPCImpl.h"
#include "stdbool.h"
#include "queue.h"
#include "RPC.h"

#define THREAD_NUM 4

#define BLOCK 1
#define DT 2
#define DH 3
#define RANDOM 4

extern void* (*g_funcList[MAX_RPC_FUNCS]) (void *args);

static int serverlen;
static pthread_mutex_t lock_wait;
static pthread_cond_t cond_wait;
static pthread_cond_t cond_wait_block;
static uint8_t  buf[MAXLINE];

static queue requests;
static queue working;
static int queue_size = THREAD_NUM;
static int listenfd, port, clientlen, threads_num, schedalg;
static struct sockaddr_in clientaddr;
bool _HandleQueueOverflow(int schedalg)
{
    bool drop = 0;
    switch (schedalg) {
        case DT:
            drop = 1;
            break;
        case DH:
            if(getQueueSize(requests) > 0 )
            {
                popFromQueue(requests);
            }
            else {
                drop = 1;
            }
            break;
        case RANDOM:
            if(getQueueSize(requests) > 0 )
            {
                removeQuarter(requests);
            }
            else {
                drop = 1;
            }
            break;
        default:
            pthread_cond_wait(&cond_wait_block,&lock_wait);
            break;
    }
    return drop;
}
void *_PerformFunction(int funcId, void *args)
{
    return g_funcList[funcId](args);
}

int RPC_SendPacket(RPC_Packet *packet){
    int n = sendto(listenfd, (const char*) packet, 4*6+packet->argSize, 0, (SA *) &clientaddr, clientlen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
    }
    return 1;
}

static void* _CallBackHandler()
{
    pthread_mutex_lock(&lock_wait);
    pthread_mutex_unlock(&lock_wait);

    while(1) {
        pthread_mutex_lock(&lock_wait);

        while(getQueueSize(requests) == 0)
        {
            pthread_cond_wait(&cond_wait,&lock_wait);
        }
        RPC_Packet *packet = getRequest(front(requests));
        popFromQueueByInt(requests,packet->packetId);
        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait_block);
        printf(" buffer here1 = %d\n", packet->argBuf[0]);
        _PerformFunction(packet->funcId, packet->argBuf);
        RPC_SendPacket(packet);

    }

}


static void* _RecieveHandler()
{
    while(1) {
        int num_byte = recvfrom(listenfd, buf, MAXBUF, 0, (SA *) &clientaddr, (socklen_t *) &clientlen);
        buf[num_byte] = '\0';
        RPC_Packet *packet = (RPC_Packet *) buf;
        printf("%d %d %d %d\n", packet->cmd, packet->funcId, packet->argSize, packet->argBuf[0]);
        //puts(buf);
        bool drop = 0;
        pthread_mutex_lock(&lock_wait);
        if (getQueueSize(requests)  >= queue_size) {
            drop = _HandleQueueOverflow(schedalg);
        }
        pthread_mutex_unlock(&lock_wait);
        if (drop) {
            continue;
        }

        while (pthread_mutex_lock(&lock_wait) != 0) {
        }
        addToQueue(requests, packet);
        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait);
    }

}

void RPC_InitThreadPool()
{
    schedalg = BLOCK;
    pthread_cond_init(&cond_wait_block,NULL);

    pthread_cond_init(&cond_wait,NULL);
    if ((pthread_mutex_init(&lock_wait, NULL) != 0) ){
        exit(1);
    }
    requests = makeQueue();
    pthread_t* threads = malloc((sizeof(pthread_t)) * (THREAD_NUM + 1));
    for(int i = 0 ; i < THREAD_NUM; i++)
    {
        pthread_create(&(threads[i]), NULL, &_CallBackHandler, NULL);
    }

    _RecieveHandler();
    //pthread_create(&(threads[THREAD_NUM]), NULL, &_RecieveHandler, NULL);
}

void RPC_Comm_Init(){
    int port = 5000;
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

