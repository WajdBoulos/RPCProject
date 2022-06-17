//
// Created by user on 5/27/22.
//

#ifndef RPCPROJECT_RPCCOMMON_H
#define RPCPROJECT_RPCCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define RPC_ARGS_MAX_SIZE 8192
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>

typedef struct
{
    uint32_t funcId;
    uint32_t callBackId;
    uint32_t inStructSize;
    uint32_t outStructSize;
    uint32_t packetId;
    uint8_t argBuf[RPC_ARGS_MAX_SIZE];
} RPC_Packet;

/* Simplifies calls to bind(), connect(), and accept() */
/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */

/* Misc constants */
#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */

/* Our own error-handling functions */
static inline void unix_error(char *msg) /* unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

#ifdef __cplusplus
}
#endif

#endif //RPCPROJECT_RPCCOMMON_H
