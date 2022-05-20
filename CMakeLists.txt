project (RPCProject)

#set(TARGET RPC)
#
#set(SRC_LIST
#        node.c
#        queue.c
#        server.c
#        wrappers.c
#        client.c
#        RPC.c
#        RPCImpl.c)

add_executable(server
        node.c
        queue.c
        server.c
        wrappers.c)

add_executable(client
        node.c
        queue.c
        wrappers.c
        client.c)