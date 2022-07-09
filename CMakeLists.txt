cmake_minimum_required(VERSION 3.0)

project (RPCProject)

add_compile_options(
        -Werror
)

add_executable(RPCHostExample
        Examples/Host/RPCHostExample.c
        )

add_executable(RPCDeviceExample
        Examples/Device/RPCDeviceExample.c
        )

target_link_libraries(RPCHostExample RPCHostLib)
target_link_libraries(RPCDeviceExample RPCDeviceLib)

add_library(RPCHostLib
        Common/Impl/node.c
        Common/Impl/queue.c
        HostLib/Impl/RPCHost.c)

target_link_libraries(RPCHostExample pthread)

add_library(RPCDeviceLib
        Common/Impl/node.c
        Common/Impl/queue.c
        DeviceLib/Impl/RPCDevice.c)

target_link_libraries(RPCDeviceExample pthread)
