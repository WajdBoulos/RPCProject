#include "wrappers.h"
#include "RPCImpl.h"
int serverlen;
struct sockaddr_in serveraddr;
int sockfd;
void initSocket(char *hostname, int portno){
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
int _SendPacket(RPC_Packet packet){
    serverlen = sizeof(serveraddr);
    int n = sendto(sockfd, (const char*) &packet, 4*6+packet.argSize, 0, &serveraddr, serverlen);
    if (n < 0) {
        unix_error("Open_clientfd Unix error");
        exit(1);
    }
    return 1;
}
int main(int argc, char *argv[])
{
    int portno;
    char *hostname;
    uint8_t  buf[RPC_ARGS_SIZE];
    /* check command line arguments */
    if (argc != 3) {
        fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);
    initSocket(hostname, portno);
    /* get a message from the user */
    bzero(buf, MAXLINE);
    printf("Please enter msg: ");
    fgets(buf, MAXLINE, stdin);
    RPC_Packet rpcPacket;
    rpcPacket.funcId = 1;
    rpcPacket.cmd = -1;
    memcpy(rpcPacket.argBuf,buf,1);
    rpcPacket.argSize = 2;
    rpcPacket.argBuf[0] = 5;
    rpcPacket.argBuf[1] = 100;
    rpcPacket.callBackId = -1;
    rpcPacket.packetId = 0;
    rpcPacket.retSize = -1;
    _SendPacket(rpcPacket);
    // waiting for response
    Recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)NULL, NULL);
    puts(buf);
    // close the descriptor
    Close(sockfd);
    exit(0);
}
