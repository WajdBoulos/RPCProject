//
// Created by user on 5/28/22.
//

#include "RPCDevice.h"

#include <stdio.h>
#include <stdlib.h>

static RPCFunction (s_funcList[2]);

void testFunc(void *in)
{
    (void)in;
    printf("yoooo this works! Server %d\n", *(int*)in);
}
#define FIBONACCI_MAX_N 1000

typedef struct
{
   int n;
} FibonaciIn;

typedef struct
{
    int n;
    int out[FIBONACCI_MAX_N];
} FibonaciOut;


void fib(void* inOut)
{
    FibonaciIn *fibI = (FibonaciIn *)inOut;
    FibonaciOut *fibO = (FibonaciOut *)inOut;
    /* Declare an array to store Fibonacci numbers. */
    int *f = fibO->out;
    int i;

    /* 0th and 1st number of the series are 0 and 1*/
    f[0] = 0;
    f[1] = 1;

    for (i = 2; i <= fibI->n; i++)
    {
        /* Add the previous 2 numbers in the series
           and store it */
        f[i] = f[i-1] + f[i-2];
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage:./RPCDeviceExample <Port>");
        return 1;
    }

    //s_funcList[0] = testFunc;
    s_funcList[0] = fib;
    RPC_Init(s_funcList, 1, atoi(argv[1]));

    while(1);
}