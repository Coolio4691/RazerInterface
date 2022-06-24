#!/bin/bash

mkdir -p build

cmake --build ./build --config Debug --target all -j 8 --
cd build
sudo ninja install

cd ..