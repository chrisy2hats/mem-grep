**Build the unit tests**  
The unit tests have 2 additional dependencies, Catch2 and NASM  
Ensure you have all the dependencies to compile mem-grep listed here - [Building](Building.md)
Then proceed with these instructions  

**Catch2**  
The testing framework in use, Catch2 is available as a Git submodule and should be fetched by running  
``
git submodule update --init
``  
  
**Install NASM**  
NASM can be installed via your distributions package manager  
**Ubuntu/Debian**  
``
sudo apt update && sudo apt install nasm  
``  
**Fedora**  
``
yum update -y && yum install nasm
``  
**OpenSuse**  
``
zypper update -y && zypper install nasm
``  
**ArchLinux**  
``
pacman --noconfirm -Syu nasm
``  
To build the unit tests run CMake with -DBUILD_UNIT_TESTS=ON  
``
cmake -DBUILD_UNIT_TESTS=ON .
``

Then compile the unit tests and the associated programs via  
``
make -j $(nproc)
``
  
**Running the unit tests**  
If the previously run make command was successful there should be a binary called "unit-tests" in the out directory  
Ensure your system is correctly configured outlined in here - [System Configuration](SystemConfiguration.md)
Then run the unit tests via  
``
./out/unit-tests
``