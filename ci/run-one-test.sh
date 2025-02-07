#! /bin/bash

# Copyright 2025 Andy Fingerhut
# SPDX-License-Identifier: Apache-2.0

# For a list of test names you can run with this command, after
# installing open-p4studio:

# cd open-p4studio
# ls install/share/tofinopd

TESTNAME="$1"

./run_tofino_model.sh -p ${TESTNAME} --arch tofino -q |& sed 's/^/model: /' &
./run_switchd.sh -p ${TESTNAME} --arch tofino |& sed 's/^/switchd: /' &
timeout 10800 ./run_p4_tests.sh -p ${TESTNAME} --arch tofino |& sed 's/^/tests: /'

echo "Killing bf_switchd and tofino-model processes ..."
sudo killall bf_switchd
sudo killall tofino-model

echo ""
echo "You may need to use the following command if"
echo "you are not seeing keystrokes echoed on this terminal:"
echo "     reset"
echo ""
