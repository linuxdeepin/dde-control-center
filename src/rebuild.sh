#!/bin/bash

rm -rf build
mkdir build
cd build
qmake ..
make
sudo make install
