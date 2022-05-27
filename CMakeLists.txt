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

add_executable(clientRPC
        node.c
        queue.c
        wrappers.c
        ClientMain.c
        RPCClientImpl.c
        RPC.c)

add_executable(ServerRPC
        node.c
        queue.c
        wrappers.c
        ServerMain.c
        RPCServerImpl.c
        RPC.c)

target_link_libraries(server pthread)
target_link_libraries(client pthread)
target_link_libraries(clientRPC pthread)
target_link_libraries(ServerRPC pthread)

