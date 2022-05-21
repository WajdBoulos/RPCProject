#include "wrappers.h"
#include "queue.h"
#include "RPCImpl.h"

#define BLOCK 1
#define DT 2
#define DH 3
#define RANDOM 4


pthread_mutex_t lock_wait;
pthread_cond_t cond_wait;
pthread_cond_t cond_wait_block;
int id_thread = 0;
queue requests;
queue working;

void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}

int getOverloadHandling(char* handler_name) {
    int overload;
    if (!strcmp(handler_name,"block")) {
        overload = BLOCK;
    }
    else if (!strcmp(handler_name,"dt")) {
        overload = DT;
    }
    else if (!strcmp(handler_name,"dh")) {
        overload = DH;
    }
    else {
        overload = RANDOM;
    }
    return overload;
}
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

int listenfd, connfd, port, clientlen, queue_size, threads_num, schedalg;
struct sockaddr_in clientaddr;
char buf[MAXLINE];

int _SendPacket(RPC_Packet *packet){
    int n = sendto(listenfd, (const char*) packet, 4*6+packet->argSize, 0, (SA *) &clientaddr, clientlen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
        exit(1);
    }
    return 1;
}

void* handler()
{

    pthread_mutex_lock(&lock_wait);
    id_thread++;
    pthread_mutex_unlock(&lock_wait);

    while(1) {
        pthread_mutex_lock(&lock_wait);

        while(getQueueSize(requests) == 0)
        {
            pthread_cond_wait(&cond_wait,&lock_wait);
        }
        RPC_Packet *packet = getRequest(front(requests));
        popFromQueue(requests);
        addToQueue(working,packet);
        pthread_mutex_unlock(&lock_wait);
        packet->retSize = 1;
        packet->callBackId = 5;

        _SendPacket(packet);

        pthread_mutex_lock(&lock_wait);
        Close(connfd);
        popFromQueueByInt(working,packet->packetId);
        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait_block);
    }

}

void _InitServer(int argc, char *argv[])
{
    struct sockaddr_in serveraddr;
    getargs(&port, argc, argv);
    srand(time(0));
    schedalg = getOverloadHandling(argv[4]);

    pthread_cond_init(&cond_wait_block,NULL);

    pthread_cond_init(&cond_wait,NULL);
    if ((pthread_mutex_init(&lock_wait, NULL) != 0) ){
        exit(1);
    }

    queue_size = atoi(argv[3]);
    threads_num = atoi(argv[2]);
    pthread_t* threads = malloc((sizeof(pthread_t)) * threads_num);
    requests = makeQueue();
    working = makeQueue();
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
    for(int i = 0 ; i < threads_num; i++)
    {
        pthread_create(&(threads[i]), NULL, &handler, NULL);
    }
}

void _RecieveHandler()
{
    while (1) {
        int num_byte = recvfrom(listenfd, buf, MAXBUF,0, (SA *)&clientaddr, (socklen_t *) &clientlen);
        buf[num_byte] = '\0';
        RPC_Packet *packet = (RPC_Packet*)buf;
        printf("%d %d %d %d %d\n", packet->cmd, packet->funcId, packet->argSize, packet->argBuf[0], packet->argBuf[1]);
        //puts(buf);
        bool drop = 0;
        pthread_mutex_lock(&lock_wait);
        if ((getQueueSize(requests) + getQueueSize(working)) >= queue_size)
        {
            drop = _HandleQueueOverflow(schedalg);
        }
        pthread_mutex_unlock(&lock_wait);
        if (drop){
            continue;
        }

        while (pthread_mutex_lock(&lock_wait)!= 0) {
        }
        addToQueue(requests, packet);

        pthread_mutex_unlock(&lock_wait);
        pthread_cond_signal(&cond_wait);
    }
}

int main(int argc, char *argv[])
{

    _InitServer(argc, argv);
    _RecieveHandler();
}


    


 
