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

/**
 * This is the initialization function of the module. This is the first function of the module that should be called in the host.
 * It initializes the callback function pointer array of the host, the communication between the host and device, and the thread pool in the host.
 *
 * @param funcArr array of callback function pointers that take void* and return void. use the typedef of RPCFunction for simplicity
 * @param numFuncs number of callback functions in the host
 * @param deviceIP IP address of the device (e.g raspberry pi)
 * @param portNum port number for the communication between the host and the device (can choose something like 5000)
 * @return returns RPC_SUCCESS upon success and RPC_FAILURE upon failure
 */
RPC_ReturnStatus RPC_Init(RPCFunction* funcArr, const int numFuncs, char* deviceIP, int portNum);

/**
 * Calls the <funcId> function in the device, and runs <callBackId> function in the host in the background once a response from the device is recieved. <args> is used as an argument buffer for both
 * the function and the callback. This function is non-blocking, but there is only one case where it blocks: when there are MAX_WINDOWS_NUM (defined in RPCHost.c) concurrent unfinished jobs, in this case the function
 * waits till WINDOW_UNLOCK_SIZE jobs are finished
 *
 * @param funcId the Id of the function to be called in the device: the index of the funcArr array in the device
 * @param callBackId the Id of the corresponding callback function to be called in the host: the index of the callback function in funcArr in the host
 * @param args buffer containing the arguments of the function to be called in the device. a maximum of 8192kb is allowed. the same buffer is also used as an output
 * @param inStructSize size of the input arguments struct (excluding output arguments size)
 * @param outStructSize size of the struct to be passed to the callback function in the host (same struct as the input + the output)
 * @return returns RPC_SUCCESS upon success and RPC_FAILURE upon failure
 */
RPC_ReturnStatus RPC_CallFunction(int funcId, int callBackId, void *args, int inStructSize, int outStructSize);

/**
 * Blocks until all unfinished jobs are finished, can be thought of like a 'job synchronization'. If waited more than timeout, the function quits and returns failure..
 * Example cases hen we can reach a timeout failure: Packet loss, One of the jobs got stuck (in the host or device), The device died
 *
 * @param timeoutSecs the function quits if timeout was exceeded, indicating failure. Time units are in seconds.
 * @return returns RPC_SUCCESS once all remaining jobs finished in timee, RPC_FAILURE if we reached timeout (we have job-loss)
 */
RPC_ReturnStatus RPC_Barrier(double timeoutSecs);

/**
 * The last function to be used in the module, it destroys all the threads used by this module and de-allocates their resources.
 */
void RPC_Destroy();

#ifdef __cplusplus
}
#endif
#endif //RPCPROJECT_RPC_CLIENT_H
