# RPCProject
The project contains 4 targets:
1. RPCHostLib
2. RPCDeviceLib
3. RPCHostExample
4. RPCDeviceExample

Build system is CMAKE (Which generates makefile files)
You can build in 2 different ways:
1. You can use Clion (which uses cmake by default) and generates the targets by default
2. you can install and use cmake manually:
   1. Install cmake via: "sudo apt install cmake" in the terminal
   2. Run terminal from RPCProject directory and type "cmake ." to generate a makefile file
   3. You will now have a makefile file generated, you can build as usual via make, e.g:
   4. "make RPCHostLib" or "make RPCDeviceExample"
