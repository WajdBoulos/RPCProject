//
// Created by user on 5/28/22.
//

#include "RPC.h"

#include <stdio.h>
#include <stdlib.h>

static void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

void testFunc(void *in)
{
    (void)in;
    printf("yoooo this works! Server %d\n", *(int*)in);
}

int main()
{
    s_funcList[0] = testFunc;
    RPC_Init(s_funcList, 1);

    while(1);
}