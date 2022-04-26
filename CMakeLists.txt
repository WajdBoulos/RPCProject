project (RPCProject)

set(TARGET RPC)

set(SRC_LIST
        node.c
        queue.c
        server.c
        wrappers.c
        client.c
        RPC.c
        RPCImpl.c)

add_library(TARGET ${SRC_LIST})