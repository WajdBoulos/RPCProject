#include "RPCHost.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_CALLBACKS_HOST 3
static RPCFunction (s_funcList[NUM_CALLBACKS_HOST]);

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

    RPC_Init(s_funcList, NUM_CALLBACKS_HOST, argv[1], atoi(argv[2]));

    for(int i = 1; i < 20; i++)
    {
        int fibonaciLen = i;
        FibonaciIn fibIn = {fibonaciLen};
        RPC_CallFunction(0, 0, &fibIn, sizeof(fibIn), sizeof(int) * (1 + fibonaciLen));
    }

    for(int i = 1; i < 20; i++)
    {
        int fibonaciLen = i;
        FibonaciIn fibIn = {fibonaciLen};
        RPC_CallFunction(0, 1, &fibIn, sizeof(fibIn), sizeof(int) * (1 + fibonaciLen));
    }

    RPC_Barrier(1);

    struct timeval t0, t1, dt;
    gettimeofday(&t0, NULL);

    const int numEmptyJobs = 1000;
    for(int i = 0; i < numEmptyJobs; i++)
    {
        RPC_CallFunction(1, 2, NULL, 0, 0);
    }

    RPC_Barrier(1);
    gettimeofday(&t1, NULL);
    timersub(&t1, &t0, &dt);

    double totTime = (1e+6 *  (double)dt.tv_sec +  (double)dt.tv_usec)/(double)numEmptyJobs;
    printf( "Time to finish empty job and callback: %f microseconds\n", totTime);

    for(int i = 1; i < 5; i++)
    {
        int fibonaciLen = i;
        FibonaciIn fibIn = {fibonaciLen};
        RPC_CallFunction(0, 1, &fibIn, sizeof(fibIn), sizeof(int) * (1 + fibonaciLen));
    }

    RPC_Barrier(1);

    RPC_Destroy();
}