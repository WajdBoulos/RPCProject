#include "wrappers.h"

/************************** 
 * Error-handling functions
 **************************/
/* $begin errorfuns */
/* $begin unixerror */
void unix_error(char *msg) /* unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}
/* $end unixerror */



/********************************
 * Wrappers for Unix I/O routines
 ********************************/



void Close(int fd) 
{
    int rc;

    if ((rc = close(fd)) < 0)
        unix_error("Close error");
}



/**************************** 
 * Sockets interface wrappers
 ****************************/



int Recvfrom(int sockfd, void *buf, size_t len, int flags,
             struct sockaddr *src_addr, socklen_t *addrlen)
{
    return recvfrom(sockfd, buf, len, flags,(struct sockaddr *)&src_addr,&addrlen);
}



