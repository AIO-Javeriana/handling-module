# Handling-module Intermediate

This intermediate module, is working as interface communication between Actor brain and microcontroller. Was developed in with C++ 11 and using [socket.io-client-cpp](https://github.com/socketio/socket.io-client-cpp) for communication channel.

### Check version cmake
> Required cmake 3.1 or later.

    $ cmake -version

## First option Installation

### Install dependencies    
> install library from repository

    $ sudo apt-get install cmake libblkid-dev e2fslibs-dev libboost-all-dev libaudit-dev

### Install Handling Module c++ 11
    $ git clone https://github.com/AIO-Javeriana/handling-module.git
    $ cd handling-module/handling-module-ODROID
    $ cmake
    $ make
    $ ./mobility_module

## Second option Installation

### Unistall cmake(if is necessary)
    $ sudo apt-get remove cmake
    $ sudo apt-get autoremove
### Install cmake 3.4(if is necessary)
    $ wget http://www.cmake.org/files/v3.4/cmake-3.4.1.tar.gz
    $ tar -xvzf cmake-3.4.1.tar.gz
    $ cd cmake-3.4.1/
    $ ./configure
    $ make
    $ sudo make install
    $ update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force

### Install Handling Module c++ 11

    $ git clone https://github.com/AIO-Javeriana/handling-module.git
    $ cd handling-module/handling-module-ODROID
    $ wget https://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.gz
    $ tar -xzvf boost_1_58_0.tar.gz
    $ rm boost_1_58_0.tar.gz
    $ cmake -DBOOST_INCLUDEDIR=./boost_1_58_0 -DBOOST_LIBRARYDIR=./boost_1_58_0/libs -DBOOST_VER:STRING=1.58 .
    $ make
    $ ./mobility_module

Happy coding!
