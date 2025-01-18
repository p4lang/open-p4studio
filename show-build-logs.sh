#! /bin/bash

# Copyright (C) 2025 Andy Fingerhut
# SPDX-License-Identifier: Apache-2.0

# Basic system info
uname -a
nproc
cat /proc/meminfo

echo "------------------------------------------------------------"
echo "Occurrences of 'killed process' in dmesg logs are a likely sign"
echo "that a process was killed by the kernel due to the system running"
echo "our of memory, leading to unpredictable build failures."
echo "------------------------------------------------------------"
sudo dmesg -T | egrep -i 'killed process'

echo "------------------------------------------------------------"
echo "Full output of command: dmesg -T"
echo "------------------------------------------------------------"
sudo dmesg -T

for f in p4studio/logs/*
do
    echo "------------------------------------------------------------"
    echo "Contents of log file: $f"
    echo "------------------------------------------------------------"
    cat $f
done
