#!/bin/sh
set -e
git clone https://github.com/nanomsg/nnpy.git
cd nnpy
sudo python3 -m pip install cffi
sudo python3 -m pip install --upgrade cffi
sudo python3 -m pip install .
cd ..
