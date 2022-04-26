//
// Created by wajd_ on 4/9/2022.
//

#include "RPC.h"

#include <string.h>
#include <stdint.h>

#define RPC_ARGS_SIZE 20
typedef enum
{
    CALL_FUNCTION,
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

void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

RPC_ReturnStatus RPC_Functions_Init(void* *funcArr(void *), const int numFuncs)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    return RPC_SUCCESS;
}

static inline RPC_Packet _CreatePacket(int command, int funcId, int callBackId, void *args, int argSize, int retSize)
{
    static uint32_t packetId = 0;
    RPC_Packet packet;
    packet.funcId = funcId;
    packet.cmd = command;
    packet.callBackId = callBackId;
    packet.argSize = argSize;
    packet.retSize = retSize;
    packet.packetId = packetId++;
    memcpy(packet.argBuf, args, argSize);
    return packet;
}

RPC_ReturnStatus _SendPacket(RPC_Packet packet);

void *_PerformFunction(int funcId, void *args)
{
    return s_funcList[funcId](args);
}

static void _ParsePacket(RPC_Packet packet)
{
    if(packet.cmd == CALL_FUNCTION)
    {
        _PerformFunction(packet.funcId, packet.argBuf);
    }
}

RPC_ReturnStatus RPC_CallFunction_Perform(int funcId, int callBackId, void *args, int argSize, int retSize, void **callBackResPtr)
{
    RPC_Packet packet = _CreatePacket(CALL_FUNCTION, funcId, callBackId, args, argSize, retSize);
    RPC_ReturnStatus rc = _SendPacket(packet);
    //wait for result
    *callBackResPtr = _PerformFunction(callBackId, args);
    return rc;
}
