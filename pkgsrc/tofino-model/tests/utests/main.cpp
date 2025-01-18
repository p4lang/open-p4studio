/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_IFACE        "udp|send:127.0.0.1:30001"
#define TEST_OF_PORT_NUM  1

#define CHECK(expr) \
    do { if (!(expr)) { assert_report(__LINE__, # expr); } } while (0)

#ifdef __cplusplus
extern "C" {
#endif

extern int common_c_gtest_main(int argc, char **argv);
extern int common_c_f(int x, int *y);

#ifdef __cplusplus
}
#endif

int main(int argc, char* argv[])
{
  printf("common utest calling gtest_main (argc=%d)\n", argc);
  return common_c_gtest_main(argc, argv);
}

