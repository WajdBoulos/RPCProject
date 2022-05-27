#include "wrappers.h"
#include "RPCImpl.h"
#include "stdbool.h"
#include "queue.h"
#include "RPC.h"

#define THREAD_NUM 4

extern void* (*g_funcList[MAX_RPC_FUNCS]) (void *args);

static int serverlen;
static struct sockaddr_in serveraddr;
static int sockfd;
static pthread_mutex_t lock_wait;
static pthread_cond_t cond_wait;
static pthread_cond_t cond_wait_block;
static uint8_t  buf[MAXLINE];

static queue requests;
static int queue_size = THREAD_NUM;

void *_PerformFunction(int funcId, void *args)
{
    return g_funcList[funcId](args);
}

int RPC_SendPacket(RPC_Packet *packetIn){
    serverlen = sizeof(serveraddr);
    int n = sendto(sockfd, (const char*) packetIn, 4*6+packetIn->argSize, 0, (const struct sockaddr_in*) &serveraddr, serverlen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
        exit(1);
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
        RPC_Packet *rpcPacket1 =  getRequest(front(requests));
        _PerformFunction(rpcPacket1->callBackId, rpcPacket1->argBuf);

        popFromQueue(requests);
        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait_block);
    }

}


static void* _RecieveHandler()
{
    RPC_Packet * packet;
    bool alive = 1;
    while(alive) {
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
    // close the descriptor
//    Close(sockfd);
//    exit(0);
}

void RPC_InitThreadPool()
{
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

    pthread_create(&(threads[THREAD_NUM]), NULL, &_RecieveHandler, NULL);
}

void RPC_Comm_Init(){
    int portno = 5000;
    char hostname[] = "132.68.60.111";
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

