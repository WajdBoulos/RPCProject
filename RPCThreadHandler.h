//
// Created by user on 5/27/22.
//

#ifndef RPCPROJECT_RPCTHREADHANDLER_H
#define RPCPROJECT_RPCTHREADHANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "RPC.h"

void RPC_Comm_Init();

void RPC_InitThreadPool();

int RPC_SendPacket(RPC_Packet *packet);

#ifdef __cplusplus
}
#endif

#endif //RPCPROJECT_RPCTHREADHANDLER_H
