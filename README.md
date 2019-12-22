mem-grep
## About  
mem-grep is a utiling for finding information about the memory layout of a running process.  
The aim is to provide a tool for the same usecase as [CheatEngine](https://www.cheatengine.org/) but for Linux  
Currently the tool focuses on finding and traversing heap pointers by exploiting the known layout of ELF binaries and the glibc heap

## Building
### Dependencies    
A C++ compiler (both g++ and clang++ tested)  
cmake (Version 3 or newer)  
GNU make  
Linux Kernel providing process_vm_readv (v3.2 onwards)    

On an Ubuntu 18.04 system these dependencies can be satisfied by running    
``
sudo apt update && sudo apt install -y g++ cmake make
``

### Compiling  
From the root of the repository run  
``
cmake . && make -j $(nproc) mem-analyze
``


**System Configuration**  
Inorder to be allowed to access a remote processes memory the program must be run as root  
Alternatively the kernel can be configured to allow attaching to non-child processes  
This can be done by running the following as root  
``
echo 0 > /proc/sys/kernel/yama/ptrace_scope
``

## Unit tests  
Unit tests are written using the Catch2 framework  
Catch2 is a submodule to this repository so to download it run  
``
git submodule update --init
``
The unit tests have additional dependencies to building the main program  
The unit tests also require nasm and GNU ld  
To install these on Ubuntu 18.04 run  
``
sudo apt update && sudo apt install -y nasm binutils  
``

Once Catch2 is cloned and depencies satisfied the unit tests can be built via  
``
cmake . && make -j $(nproc) unit-tests bssHeapPointers runUntilManipulatedStack runUntilManipulatedHeap asmTarget  
``  
This will output the "unit-tests" binary and other required binaries in the "out" directory  
The unit tests can then be run via  
``
./out/unit-tests
``
