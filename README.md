mem-grep
## About  
mem-grep is a utiling for finding information about the memory layout of a running process.  
The aim is to provide a tool for the same usecase as [CheatEngine](https://www.cheatengine.org/) but for Linux  
Currently the tool focuses on finding and traversing heap pointers by exploiting the known layout of ELF binaries and the glibc heap

## Building - Backend
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
## Building - GUI  
## On an Ubuntu 18.04 system the GUI build dependencies can be satisfied by running  
``
sudo apt update && sudo apt install qtbase5-dev qt5-qmake build-essential qt5-default  
``

## Compile GUI  
``
qmake src/GUI/mem-grep.pro && make  
``

There should then be a mem-grep-gui binary in your current directory  

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

Once Catch2 is cloned and dependencies satisfied the unit tests can be built via  
``
cmake -DBUILD_UNIT_TESTS=ON . && make -j $(nproc) asmTarget  bssHeapPointers multiLayeredBssHeapPointers  oneTwoSevenOnStack  onheapint  onstackint  runUntilManipulatedHeap  unit-tests
``  
This will output the "unit-tests" binary and other required binaries in the "out" directory  
The unit tests can then be run via  
``
cd out && ./unit-tests
``

EULA  
This software should not be used to manipulate the memory of any program that is communicating with third parties such as games connected to a game server as this may negatively impact the experience of other players.


