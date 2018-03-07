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
mbed new mbed-test --mbedlib
cd mbed-test
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
