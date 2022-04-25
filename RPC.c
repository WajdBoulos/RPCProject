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
    uint32_t packetId;
    uint8_t argBuf[RPC_ARGS_SIZE];
} RPC_Packet;

void (*s_funcList[MAX_RPC_FUNCS]) (void *in, void *out);

RPC_ReturnStatus RPC_Functions_Init(void *funcArr(void *, void *), const int numFuncs)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    return RPC_SUCCESS;
}

static inline RPC_Packet _CreatePacket(int funcId, void *inArgs, RPC_Command command)
{
    static uint32_t packetId = 0;
    RPC_Packet packet;
    packet.funcId = funcId;
    packet.cmd = command;
    packet.packetId = packetId++;
    memcpy(packet.argBuf, inArgs, sizeof(packet.argBuf));
    return packet;
}

void _SendPacket(RPC_Packet packet);

void _PerformFunction(int funcId, void * args)
{
    s_funcList[funcId](args, args);
}

static void _ParsePacket(RPC_Packet packet)
{
    if(packet.cmd == CALL_FUNCTION)
    {
        _PerformFunction(packet.funcId, packet.argBuf);
    }
}

RPC_ReturnStatus RPC_CallFunction_Perform(int funcId, void *inArgs, void *outArgs)
{
    RPC_Packet packet = _CreatePacket(funcId, inArgs, CALL_FUNCTION);
    _SendPacket(packet);
    return RPC_SUCCESS;
}
