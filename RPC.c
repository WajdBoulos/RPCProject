//
// Created by wajd_ on 4/9/2022.
//

#include "RPC.h"

#include <string.h>

void (*s_funcList[MAX_RPC_FUNCS]) (void *in, void *out);

RPC_ReturnStatus RPC_Functions_Init(void *funcArr(void *, void *), const int numFuncs)
{
    if(numFuncs > MAX_RPC_FUNCS)
    {
        return RPC_FAILURE;
    }

    memcpy(s_funcList, funcArr, sizeof(funcArr) * numFuncs);
    return RPC_SUCCESS;
}

