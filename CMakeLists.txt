project (RPCProject)

add_executable(RPCHostExample
        node.c
        queue.c
        RPCHostExample.c
        RPCHost.c
        )

add_executable(RPCDeviceExample
        node.c
        queue.c
        RPCDeviceExample.c
        RPCDevice.c)

target_link_libraries(RPCHostExample pthread)
target_link_libraries(RPCDeviceExample pthread)

