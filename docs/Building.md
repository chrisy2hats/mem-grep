**Building**  
It should be possible to compile and run mem-grep on any modern Linux distribution  
Builds have been tested on Ubuntu 18.04, Debian Buster, Fedora 31, OpenSuse Tumbleweed and ArchLinux  
If you want to be able to run the unit tests also refer to the documentation - Running Unit Tests(TODO)

**Dependencies**  
A C++17 compiler (both gcc and clang tested)  
CMake version >= 3.0  
GNU make version >= 4.1  
glibc version >= 2.15  
Linux Kernel version >= 3.2  

**Install Dependencies**  
  
**Ubuntu/Debian**  
on an Ubuntu 18.04 or Debian Buster system the dependencies can be satisfied by running  
``
sudo apt update && sudo apt install -y g++ cmake make
``  
  
**Fedora**  
on Fedora 31 the dependencies can be satisfied by running  
``  
sudo yum update -y && sudo yum install -y cmake make gcc g++ procps 
``  
  
**OpenSuse**  
on OpenSuse tumbleweed the dependencies can be satisfied by running  
``
sudo zypper update -y && sudo zypper install -y cmake make gcc-c++ 
``
  
**ArchLinux**  
On ArchLinux the dependencies can be satisfied by running  
``
sudo pacman -Syu git gcc cmake make 
``
  
**Compiling**  
The process is the same for all distributions  
From the root of the repository run  
``
cmake . -DCMAKE_BUILD_TYPE=Release && make -j $(nproc) mem-grep
``
  
**After building**  
Once the program is compiled refer to the configure your system as explained in the System Configuration page(TODO)
