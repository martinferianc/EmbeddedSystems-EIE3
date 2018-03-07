## Installation Instructions
# install mbed
```
pip install mbed-cli
```
# install mecurial
```
sudo apt install mercurial
```

# install gcc_arm toolchain
```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt-get update
sudo apt-get install gcc-arm-embedded
```
# create a new mbed-os2 project 
```
mbed new src --mbedlib
cd src
```
> note: need to add a main.cpp here

# add required libraries to project
```
mbed add https://os.mbed.com/users/mbed_official/code/mbed-dev/
mbed add https://os.mbed.com/users/mbed_official/code/mbed-rtos/
mbed add https://os.mbed.com/users/feb11/code/Crypto/
```

# compile project
```
mbed compile -t GCC_ARM -m NUCLEO_F303K8
```

A new mbed project will need to be initialised. Make sure to copy in the .gitignore as well, so that the libraries and compiled bitstreams are not included in the repo.
