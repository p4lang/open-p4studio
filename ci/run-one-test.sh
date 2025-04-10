#! /bin/bash

# Copyright 2025 Andy Fingerhut
# SPDX-License-Identifier: Apache-2.0
#
# For a list of test names that you can run with this command,
# after installing open-p4studio:
#
# cd open-p4studio
# ls install/share/tofinopd       # For Tofino1 tests
# ls install/share/tofino2pd      # For Tofino2 tests
#
# This script only supports running one test on a system at a time,
# not multiple in parallel.  This significantly simplifies the
# development of this script.

print_help() {
    echo "usage: $(basename ""$0"") [ --arch <tofino|tofino2> ] -p <prog-name>"
}

opts=`getopt -o p: --long arch: -- "$@"`
if [ $? != 0 ]; then
  exit 1
fi
eval set -- "$opts"

P4_NAME=""
ARCH="tofino"

while true; do
    case "$1" in
      -p) P4_NAME=$2; shift 2;;
      --arch) ARCH=$2; shift 2;;
      --) shift; break;;
    esac
done

if [ -z $P4_NAME ]
then
    print_help
    exit 1
fi

OUTF=`mktemp -t tofino-model-output-XXXXXX.txt`
./run_tofino_model.sh -p ${P4_NAME} --arch ${ARCH} -q |& tee ${OUTF} |& sed 's/^/model: /' &
MODEL_PID=$!
sleep 3
# Check if model process is still running.
if ps -p ${MODEL_PID} > /dev/null
then
    echo "tofino-model process is still running.  Continuing."
else
    echo "tofino-model process has exited quickly.  Output is in file ${OUTF}, and copied below:"
    echo "----------------------------------------"
    cat ${OUTF}
    echo "----------------------------------------"
    exit 1
fi
./run_switchd.sh -p ${P4_NAME} --arch ${ARCH} |& sed 's/^/switchd: /' &
timeout 10800 ./run_p4_tests.sh -p ${P4_NAME} --arch ${ARCH} |& sed 's/^/tests: /'
test_exit_status=${PIPESTATUS[0]}

# This script only supports running one test on a system at a time,
# not multiple in parallel.  Thus killing all of these processes is
# OK.
sudo killall bf_switchd tofino-model

echo ""
echo "You may need to use the following command if"
echo "you are not seeing keystrokes echoed on this terminal:"
echo "     reset"
echo ""
echo "Exiting with status ${test_exit_status}"
exit ${test_exit_status}
