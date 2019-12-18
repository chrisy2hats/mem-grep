mem-grep

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

Once the clone is finished the unit tests can be built via  
``
cmake . && make -j $(nproc) unit-tests  
``  
This will output the "unit-tests" binary in the "out" directory.  
This binary can be run to execute the unit tests via  
``
./out/unit-tests
``
