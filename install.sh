#!/bin/bash

sudo apt install -y git
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
cd
git clone https://github.com/fhswf/PICO-CAM-A.git
cd PICO-CAM-A
export PICO_SDK_PATH=$(pwd)/pico-sdk



