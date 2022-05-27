#include "RPC.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void* (*s_funcList[MAX_RPC_FUNCS]) (void *args);

void *testFunc(void *in)
{
    (void)in;
    printf("yoooo this works! client %d\n", *(int*)(in) + 1);
}

int main()
{
    s_funcList[0] = testFunc;
    RPC_Init(s_funcList, 1);

    int printNum = 9;
    RPC_CallFunction(0, 0, &printNum, 4, 0, NULL);
    sleep(0.01);
    printNum = 5;

    RPC_CallFunction(0, 0, &printNum, 4, 0, NULL);
    sleep(1);

    printNum = 2;
    sleep(1);

    RPC_CallFunction(0, 0, &printNum, 4, 0, NULL);
    printNum = 1;
    sleep(1);

    RPC_CallFunction(0, 0, &printNum, 4, 0, NULL);

    while(1);
}