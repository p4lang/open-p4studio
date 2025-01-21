#! /bin/bash

# Copyright 2024 Intel Corporation

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# On x86_64 Ubuntu 20.04.6 system, with no build-essential package installed.
# Note that having at least 4 GBytes of RAM per CPU core,
# i.e. the output of the `nproc` command, is probably close
# to the minimum amount of RAM you should have for parts of the build to succeed.

# Remember the current directory when the script was started:
INSTALL_DIR="${PWD}"

THIS_SCRIPT_FILE_MAYBE_RELATIVE="$0"
THIS_SCRIPT_DIR_MAYBE_RELATIVE="${THIS_SCRIPT_FILE_MAYBE_RELATIVE%/*}"
THIS_SCRIPT_DIR_ABSOLUTE=`readlink -f "${THIS_SCRIPT_DIR_MAYBE_RELATIVE}"`

linux_version_warning() {
    1>&2 echo "Found ID ${ID} and VERSION_ID ${VERSION_ID} in /etc/os-release"
    1>&2 echo "This script only supports these:"
    1>&2 echo "    ID ubuntu, VERSION_ID in 20.04 22.04"
    1>&2 echo ""
    1>&2 echo "Proceed installing manually at your own risk of"
    1>&2 echo "significant time spent figuring out how to make it all"
    1>&2 echo "work, or consider getting VirtualBox and creating a"
    1>&2 echo "virtual machine with one of the tested versions."
}

abort_script=0

if [ ! -r /etc/os-release ]
then
    1>&2 echo "No file /etc/os-release.  Cannot determine what OS this is."
    linux_version_warning
    exit 1
fi
source /etc/os-release
PROCESSOR=`uname --machine`

supported_distribution=0
tried_but_got_build_errors=0
if [ "${ID}" = "ubuntu" ]
then
    case "${VERSION_ID}" in
	20.04)
	    supported_distribution=1
	    ;;
	22.04)
	    supported_distribution=1
	    ;;
    esac
fi

if [ ${supported_distribution} -eq 1 ]
then
    echo "Found supported ID ${ID} and VERSION_ID ${VERSION_ID} in /etc/os-release"
else
    linux_version_warning
    if [ ${tried_but_got_build_errors} -eq 1 ]
    then
	1>&2 echo ""
	1>&2 echo "This OS has been tried at least once before, but"
	1>&2 echo "there were errors during a compilation or build"
	1>&2 echo "step that have not yet been fixed.  If you have"
	1>&2 echo "experience in fixing such matters, your help is"
	1>&2 echo "appreciated."
    fi
    exit 1
fi

available_mem_MBytes() {
    local memtotal_KBytes=`head -n 1 /proc/meminfo | awk '{print $2;}'`
    local memtotal_MBytes=`expr ${memtotal_KBytes} / 1024`
    echo ${memtotal_MBytes}
}

available_processors() {
    local n=`grep -c '^processor' /proc/cpuinfo`
    echo $n
}

# When p4c is built with the unity option, at least one individual
# process uses 4.5 GBytes of RAM, and there are others that use
# 2-3 GBytes of RAM that might run in parallel with that one.
#
# To be safe, overestimate slightly using this formula for
# p4c unity builds, where N is the number of parallel jobs:
#
# expected_max_mem_usage(N) = 2 GBytes + N * (4 GBytes)
#
# If later we decide to disable unity build for p4c, I suspect
# we could use this formula instead, but I have not tested this:
#
# expected_max_mem_usage(N) = 2 GBytes + N * (2 GBytes)

expected_max_mem_usage_MBytes() {
    local num_jobs=$1
    # expected base memory required
    local a=2048
    # additional memory required for each parallel job (including 1)
    local b=4096
    local total=`expr ${a} + ${b} \* ${num_jobs}`
    echo $total
}

# max_parallel_jobs calculates a number of parallel jobs N to run for
# a command like `make -j<N>`
#
# Often this does not actually help finish the command earlier if N is
# larger than the number of CPU cores on the system, so calculate a
# value of N no more than that number.
#
# Also, if N is so large that the processes started in parallel exceed
# the available memory on the system, it can cause the system to copy
# memory to and from swap space, which dramatically reduces
# performance.  Alternately, it can cause the kernel to kill
# processes, to reduce system memory usage, which causes the overall
# job to fail.  Thus we would like to calculate a value of N such
# that this is true:
#
# expected_max_mem_usage(N) <= currently free mem

max_parallel_jobs() {
    local avail_mem_MBytes=$1
    local max_jobs_for_processors=`available_processors`
    local num_jobs=0
    local next_num_jobs
    local required_mem
    local continue=1
    while [ ${continue} -eq 1 ]
    do
        next_num_jobs=`expr ${num_jobs} + 1`
        required_mem=`expected_max_mem_usage_MBytes ${next_num_jobs}`
        if [ ${avail_mem_MBytes} -lt ${required_mem} ]
        then
            continue=0
        else
            num_jobs=${next_num_jobs}
        fi
    done
    local max_jobs_for_mem=${num_jobs}

    1>&2 echo "Available memory (MBytes): ${avail_mem_MBytes}"
    1>&2 echo "Max number of parallel jobs for available mem: ${max_jobs_for_mem}"
    1>&2 echo "Max number of parallel jobs for processors: ${max_jobs_for_processors}"
    if [ ${max_jobs_for_processors} -lt ${max_jobs_for_mem} ]
    then
	echo ${max_jobs_for_processors}
    else
	echo ${max_jobs_for_mem}
    fi
}

abort_script=0
min_mem_MBytes=`expected_max_mem_usage_MBytes 1`
avail_mem_MBytes=`available_mem_MBytes`
num_jobs=`max_parallel_jobs ${avail_mem_MBytes}`
if [ ${num_jobs} -lt 1 ]
then
    mem_comment="too low"
    abort_script=1
else
    mem_comment="enough for ${num_jobs} parallel jobs"
fi

echo "Minimum recommended memory to run this script: ${min_mem_MBytes} MBytes"
echo "Memory on this system from /proc/meminfo:      ${avail_mem_MBytes} MBytes -> $mem_comment"

if [ "${abort_script}" == 1 ]
then
    1>&2 echo ""
    1>&2 echo "Aborting script because system has too little RAM."
    exit 1
fi

get_used_disk_space_in_mbytes() {
    echo $(df --output=used --block-size=1M . | tail -n 1)
}

echo "------------------------------------------------------------"
echo "Time and disk space used before installation begins:"
DISK_USED_START=`get_used_disk_space_in_mbytes`
date
TIME_START=$(date +%s)
set -x

set +x
echo "Version of p4lang/open-p4studio repo used:"
set -x
cd "${THIS_SCRIPT_DIR_ABSOLUTE}"
git log -n 1 | head -n 3
git submodule update --init --recursive
cd .git/modules/pkgsrc/p4-compilers/p4c
set +x
echo "Version of p4lang/p4c repo used:"
set -x
git log -n 1 | head -n 3
cd "${THIS_SCRIPT_DIR_ABSOLUTE}"

./p4studio/p4studio profile apply --jobs ${num_jobs} ./p4studio/profiles/testing.yaml

set +x
echo "------------------------------------------------------------"
echo "Time and disk space used when installation was complete:"
date
TIME_END=$(date +%s)
DISK_USED_END=`get_used_disk_space_in_mbytes`
echo ""
echo "Elapsed time for various install steps:"
echo "Total time             : $(($TIME_END-$TIME_START)) sec"
echo "All disk space utilizations below are in MBytes:"
echo ""
echo  "DISK_USED_START                ${DISK_USED_START}"
echo  "DISK_USED_END                  ${DISK_USED_END}"
echo  "DISK_USED_END - DISK_USED_START : $((${DISK_USED_END}-${DISK_USED_START})) MBytes"

set +x
cd "${HOME}"
cp /dev/null setup-open-p4studio.bash
echo "export SDE=\"${THIS_SCRIPT_DIR_ABSOLUTE}\"" >> setup-open-p4studio.bash
echo "export SDE_INSTALL=\"\${SDE}/install\"" >> setup-open-p4studio.bash
echo "export LD_LIBRARY_PATH=\"\${SDE_INSTALL}/lib\"" >> setup-open-p4studio.bash
echo "export PATH=\"\${SDE_INSTALL}/bin:\${PATH}\"" >> setup-open-p4studio.bash

echo "If you use a Bash-like command shell, you may wish to add a line like"
echo "the following to your .bashrc or other shell rc file:"
echo ""
echo "    source \$HOME/setup-open-p4studio.bash"
