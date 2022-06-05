//
// Created by wajd_ on 4/9/2022.
//

#ifndef RPCPROJECT_RPC_CLIENT_H
#define RPCPROJECT_RPC_CLIENT_H

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RPC_FUNCS 100

typedef void (*RPCFunction)(void*);

typedef enum
{
    RPC_SUCCESS,
    RPC_FAILURE
} RPC_ReturnStatus;

RPC_ReturnStatus RPC_Init(RPCFunction* funcArr, const int numFuncs, char* deviceIP, int portNum);

RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int inStructSize, int outStructSize);

void RPC_Barrier();

void RPC_Destroy();

#ifdef __cplusplus
}
#endif
#endif //RPCPROJECT_RPC_CLIENT_H
