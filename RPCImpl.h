//
// Created by user on 5/27/22.
//

#ifndef RPCPROJECT_RPCIMPL_H
#define RPCPROJECT_RPCIMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#define RPC_ARGS_SIZE 8000

#include <stdint.h>
typedef enum
{
    CALL_FUNCTION,
    CALLBACK,
} RPC_Command;

typedef struct
{
    uint32_t cmd;
    uint32_t funcId;
    uint32_t callBackId;
    uint32_t argSize;
    uint32_t retSize;
    uint32_t packetId;
    uint8_t argBuf[RPC_ARGS_SIZE];
} RPC_Packet;

void RPC_Comm_Init();

void RPC_InitThreadPool();

int RPC_SendPacket(RPC_Packet *packet);

#ifdef __cplusplus
}
#endif

#endif //RPCPROJECT_RPCIMPL_H
