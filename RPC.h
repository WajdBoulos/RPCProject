//
// Created by wajd_ on 4/9/2022.
//

#ifndef RPCPROJECT_RPC_H
#define RPCPROJECT_RPC_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RPC_FUNCS 100

typedef enum
{
    RPC_SUCCESS,
    RPC_FAILURE
} RPC_ReturnStatus;

RPC_ReturnStatus RPC_Functions_Init(void* *funcArr(void *args), const int numFuncs);

RPC_ReturnStatus RPC_Function_Call(const int funcId, const int numFuncs);

#ifdef __cplusplus
}
#endif
#endif //RPCPROJECT_RPC_H
