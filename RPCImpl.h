//
// Created by wajd_ on 4/26/2022.
//

#ifndef RPCPROJECT_RPCIMPL_H
#define RPCPROJECT_RPCIMPL_H

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


#endif //RPCPROJECT_RPCIMPL_H
