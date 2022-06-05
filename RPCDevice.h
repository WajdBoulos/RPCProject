
#ifndef RPCPROJECT_RPC_DEVICE_H
#define RPCPROJECT_RPC_DEVICE_H

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
/**
 *
 * */
RPC_ReturnStatus RPC_Init(RPCFunction* funcArr, const int numFuncs, int portNum);

#ifdef __cplusplus
}
#endif
#endif //RPCPROJECT_RPC_DEVICE_H
