
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
 * This is the initialization function of the module. This is the first function of the module that should be called in the device.
 * It initializes the function pool of the device, the communication between the host and device, and the thread pool in the device.
 *
 * @param funcArr  array of function pointers that take void* and return void. use the typedef of RPCFunction for simplicity
 * @param numFuncs number of functions in the device
 * @param portNum port number for the communication between the host and the device (can choose something like 5000)
 * @return
 */
RPC_ReturnStatus RPC_Init(RPCFunction* funcArr, const int numFuncs, int portNum);

#ifdef __cplusplus
}
#endif
#endif //RPCPROJECT_RPC_DEVICE_H
