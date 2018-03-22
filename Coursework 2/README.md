## Specification
The objective of this task was to implement a controller which would enable us to drive a brush-less direct current (DC) motor. The controller should allow us to perform functions you would expect from a standard motor controller, such as spin at a fixed velocity and rotate for a set number of rotations.

## Structure
```
.
├── FINAL_BUILD
├── bitcoinMine.cpp
├── bitcoinMine.h
├── build_profile1.json
├── compile_linux.sh
├── compile_mac.sh
├── decodeCommands.cpp
├── decodeCommands.h
├── main.cpp
├── messages.cpp
├── messages.h
├── motorControl.cpp
├── motorControl.h
├── requirements.txt
└── threadHeader.h
```
`FINAL_BUILD` contains the final executable. `bitcoinMine.cpp/h` contain functions to enable hashing and counting of nonches or hashes. `decodeCommands.cpp/h` contain threaded functions to decode the input from the serial communication. `messages.cpp/h` contain functions to process the the incomming and outcoming communication. Motor control is done inside `motorControl.cpp/h`. Everything is then encapsulated inside the threads and a main inside `main.cpp`

## Building & Running

If you would like to only run the code, copy the executable inside `FINAL_BUILD`.

### Building on Linux
#### Install mbed
```
cd src/
pip install mbed-cli
```
#### Install mecurial
```
sudo apt install mercurial
```
#### Install gcc_arm toolchain
```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt-get update
sudo apt-get install gcc-arm-embedded
```
####  Create a new mbed-os2 project
```
mbed new . --mbedlib
```
#### Add required libraries to project
```
mbed add https://os.mbed.com/users/mbed_official/code/mbed-dev/
mbed add https://os.mbed.com/users/mbed_official/code/mbed-rtos/
mbed add https://os.mbed.com/users/feb11/code/Crypto/
```
#### Compile project
```
mbed target NUCLEO_F303K8
mbed compile -t GCC_ARM --profile=build_profile1.json
```

### Building on Linux
#### Install Brew Cask
```
brew tap caskroom/cask
```
#### Install mbed
```
pip install mbed-cli
```
#### Install mecurial
```
brew install mercurial
```
#### Install gcc_arm toolchain
```
brew cask gcc-arm-embedded
brew tap osx-cross-arm
brew install arm-gcc-bin
```
#### Create a new mbed-os2 project
```
mbed new . --mbedlib
```
#### Add required libraries to project
```
mbed add https://os.mbed.com/users/mbed_official/code/mbed-dev/
mbed add https://os.mbed.com/users/mbed_official/code/mbed-rtos/
mbed add https://os.mbed.com/users/feb11/code/Crypto/
```
#### Compile project
```
mbed target NUCLEO_F303K8 // sets the default target to our board
mbed compile -t GCC_ARM
```
