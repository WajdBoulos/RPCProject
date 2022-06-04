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

add_executable(RPCHostExample
        node.c
        queue.c
        wrappers.c
        ClientMain.c
        RPCHost.c
        )

add_executable(RPCDeviceExample
        node.c
        queue.c
        wrappers.c
        ServerMain.c
        RPCDevice.c)

target_link_libraries(server pthread)
target_link_libraries(client pthread)
target_link_libraries(RPCHostExample pthread)
target_link_libraries(RPCDeviceExample pthread)

