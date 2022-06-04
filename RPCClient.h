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

typedef enum
{
    RPC_SUCCESS,
    RPC_FAILURE
} RPC_ReturnStatus;

RPC_ReturnStatus RPC_Init(void* *funcArr(void *), const int numFuncs, char* deviceIP, int portNum);

RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int argSize, int retSize, void **callBackResPtr);

void RPC_Barrier();


#ifdef __cplusplus
}
#endif
#endif //RPCPROJECT_RPC_CLIENT_H
