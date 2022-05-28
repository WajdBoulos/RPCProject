#include "RPC.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

void *testFunc(void *in)
{
    (void)in;
    printf("yoooo this works! client %d\n", *(int*)(in));
}

int main()
{
    s_funcList[0] = testFunc;
    RPC_Init(s_funcList, 1);

    int printNum1= 9;
    RPC_CallFunction(0, 0, &printNum1, 4, 0, NULL);
    //sleep(0.01);
    int printNum2 = 5;

    RPC_CallFunction(0, 0, &printNum2, 4, 0, NULL);
    //sleep(0.0011);

    int printNum3 = 2;

    RPC_CallFunction(0, 0, &printNum3, 4, 0, NULL);
    int printNum4 = 1;
    //sleep(1);

    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);
    RPC_CallFunction(0, 0, &printNum4, 4, 0, NULL);

    while(1);
}