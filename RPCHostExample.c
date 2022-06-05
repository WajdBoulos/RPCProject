#include "RPCHost.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

static RPCFunction (s_funcList[3]);

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

void printFibonaciRes(void *in)
{
    FibonaciOut *res = in;
    for(int i = 0; i < res->n; i++)
    {
        printf("%d ", res->out[i]);
    }
    printf("\n");
}

void printFibonaciResReverse(void *in)
{
    FibonaciOut *res = in;
    for(int i = res->n - 1; i >= 0; i--)
    {
        printf("%d ", res->out[i]);
    }
    printf("\n");
}

void stubFunc(void *in)
{
    (void)in;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage:./RPCHostExample <Server_IP> <Port>");
        return 1;
    }

    s_funcList[0] = printFibonaciRes;
    s_funcList[1] = printFibonaciResReverse;
    s_funcList[2] = stubFunc;

    RPC_Init(s_funcList, 3, argv[1], atoi(argv[2]));

    for(int i = 1; i < 8; i++)
    {
        int fibonaciLen = i;
        FibonaciIn fibIn = {fibonaciLen};
        RPC_CallFunction(0, 0, &fibIn, sizeof(fibIn), sizeof(int) * (1 + fibonaciLen));
    }

    for(int i = 1; i < 8; i++)
    {
        int fibonaciLen = i;
        FibonaciIn fibIn = {fibonaciLen};
        RPC_CallFunction(0, 1, &fibIn, sizeof(fibIn), sizeof(int) * (1 + fibonaciLen));
    }

    RPC_Barrier();

    clock_t begin = clock();
    RPC_CallFunction(1, 2, NULL, 0, 0);
    RPC_Barrier();
    clock_t end = clock();
    float time_spent = (float)(end - begin) / CLOCKS_PER_SEC;
    printf("Time to finish empty job and callback: %f\n", time_spent);

    RPC_Destroy();
}