//
// Created by wajd_ on 4/9/2022.
//

#include "RPC.h"
#include "RPCImpl.h"
#include <string.h>
#include <stdint.h>

#include "RPCThreadHandler.h"

void* (*g_funcList[MAX_RPC_FUNCS]) (void *args);

static uint32_t s_packetId = 0;

RPC_ReturnStatus RPC_Init(void* *funcArr(void *), const int numFuncs)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    memcpy(g_funcList, funcArr, sizeof(funcArr) * numFuncs);

    RPC_InitThreadPool();
    RPC_Comm_Init();
    return RPC_SUCCESS;
}

static inline RPC_Packet _CreatePacket(int command, int funcId, int callBackId, void *args, int argSize, int retSize)
{
    RPC_Packet packet;
    packet.funcId = funcId;
    packet.cmd = command;
    packet.callBackId = callBackId;
    packet.argSize = argSize;
    packet.retSize = retSize;
    packet.packetId = s_packetId++;
    memcpy(packet.argBuf, args, argSize);
    return packet;
}

RPC_ReturnStatus _SendPacket(RPC_Packet packet);



//static void _ParsePacket(RPC_Packet packet)
//{
//    if(packet.cmd == CALL_FUNCTION)
//    {
//        _PerformFunction(packet.funcId, packet.argBuf);
//    }
//}

RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int argSize, int retSize, void **callBackResPtr)
{
    RPC_Packet packet = _CreatePacket(CALL_FUNCTION, funcId, callBackId, args, argSize, retSize);
    RPC_ReturnStatus rc = RPC_SendPacket(&packet);
    return rc;
}
