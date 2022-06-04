//
// Created by user on 5/28/22.
//

#include "RPCDevice.h"

#include <stdio.h>
#include <stdlib.h>

static void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

void testFunc(void *in)
{
    (void)in;
    printf("yoooo this works! Server %d\n", *(int*)in);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage:./RPCDeviceExample <Port>");
        return 1;
    }

    s_funcList[0] = testFunc;
    RPC_Init(s_funcList, 1, atoi(argv[1]));

    while(1);
}