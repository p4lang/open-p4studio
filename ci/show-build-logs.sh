#! /bin/bash

# Copyright (C) 2025 Andy Fingerhut
# SPDX-License-Identifier: Apache-2.0

# Basic system info
set -x
uname -a
nproc
cat /proc/meminfo
gcc --version

set +x
echo "------------------------------------------------------------"
echo "Info about Python binaries and packages installed."
echo "------------------------------------------------------------"
set -x
python3 -V
find . -name 'python3.*'
du install/lib

set +x
for f in p4studio/logs/*
do
    set -x
    wc $f
    set +x
done

set +x
echo "------------------------------------------------------------"
echo "Occurrences of 'killed process' in dmesg logs are a likely sign"
echo "that a process was killed by the kernel due to the system running"
echo "our of memory, leading to unpredictable build failures."
echo "------------------------------------------------------------"
set -x
sudo dmesg -T | egrep -i 'killed process'

set +x
echo "------------------------------------------------------------"
echo "Full output of command: dmesg -T"
echo "------------------------------------------------------------"
set -x
sudo dmesg -T

set +x
for f in p4studio/logs/*
do
    echo "------------------------------------------------------------"
    echo "Contents of log file: $f"
    echo "------------------------------------------------------------"
    cat $f
done
