cmake_minimum_required(VERSION 3.0)

project (RPCProject)

add_compile_options(
        -O3 -Werror
)

add_executable(RPCHostExample
        RPCHostExample.c
        )

add_executable(RPCDeviceExample
        RPCDeviceExample.c
        )

target_link_libraries(RPCHostExample RPCHostLib)
target_link_libraries(RPCDeviceExample RPCDeviceLib)

add_library(RPCHostLib
        node.c
        queue.c
        RPCHost.c)

target_link_libraries(RPCHostExample pthread)

add_library(RPCDeviceLib
        node.c
        queue.c
        RPCDevice.c)

target_link_libraries(RPCDeviceExample pthread)
