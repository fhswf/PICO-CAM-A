#!/bin/bash

# run with: curl -H "Cache-Control: no-cache" -L https://t1p.de/pico-install | bash

sudo apt install -y git cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib g++
cd
git clone https://github.com/fhswf/PICO-CAM-A.git
cd PICO-CAM-A
mkdir build
cd build
cmake -D PICO_SDK_FETCH_FROM_GIT_PATH=1 ..
make

