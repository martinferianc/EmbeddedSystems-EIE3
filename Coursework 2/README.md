## Installation Instructions- Linux
# Install mbed
```
pip install mbed-cli
```
# Install mecurial
```
sudo apt install mercurial
```

# Install gcc_arm toolchain
```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt-get update
sudo apt-get install gcc-arm-embedded
```
# Create a new mbed-os2 project 
```
mbed new src --mbedlib
cd src
```
> note: need to add a main.cpp here

# Add required libraries to project
```
mbed add https://os.mbed.com/users/mbed_official/code/mbed-dev/
mbed add https://os.mbed.com/users/mbed_official/code/mbed-rtos/
mbed add https://os.mbed.com/users/feb11/code/Crypto/
```

# Compile project
```
mbed compile -t GCC_ARM -m NUCLEO_F303K8
```

## Installation Instructions- Mac

# Requirements:
  *brew
  *brew cask
  *pip

# Install Brew Cask
```
brew tap caskroom/cask
```

# Install mbed
```
pip install mbed-cli
```
# Install mecurial
```
brew install mercurial
```

# Install gcc_arm toolchain
```
brew cask gcc-arm-embedded
brew tap osx-cross-arm
brew install arm-gcc-bin
```
# Create a new mbed-os2 project 
```
mbed new src --mbedlib
cd src
```
> note: need to add a main.cpp here

# Add required libraries to project
```
mbed add https://os.mbed.com/users/mbed_official/code/mbed-dev/
mbed add https://os.mbed.com/users/mbed_official/code/mbed-rtos/
mbed add https://os.mbed.com/users/feb11/code/Crypto/
```

# Compile project
```
mbed target NUCLEO_F303K8 // sets the default target to our board
mbed compile -t GCC_ARM
```


A new mbed project will need to be initialised. Make sure to copy in the .gitignore as well, so that the libraries and compiled bitstreams are not included in the repo.
