#!/bin/bash

WHICH_TESTS="Dv De I L M S P C V T R Q O A B"
OTHER_TESTS="E F G H J K N U W X Y Z"
PASS_STR="^\[  PASSED  \]"
FAIL_STR="^\[  FAILED  \]"
TEST_BIN="./tests/utests/utests"
TEST_CMD_BASE="$TEST_BIN --gtest_filter={}*"

[ ! -x $TEST_BIN ] && exit 1
OUT_DIR=$(date '+OUT_%Y%m%d_%H%M%S_%N')
mkdir $OUT_DIR
TEST_CMD="$TEST_CMD_BASE >$OUT_DIR/{}.out"
date '+Starting parallel unit-test run at %c'
parallel --jobs -1 --progress "$TEST_CMD" ::: $WHICH_TESTS
date '+Finished parallel unit-test run at %c'
ls -l $OUT_DIR/*.out
grep "$PASS_STR" $OUT_DIR/*.out
grep "$FAIL_STR" $OUT_DIR/*.out


