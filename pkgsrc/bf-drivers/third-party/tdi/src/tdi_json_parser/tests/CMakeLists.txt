# SPDX-FileCopyrightText: 2022 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

include(CTest)
ENABLE_TESTING()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../targets)
add_executable(tdi_json_utest
  main.cpp
  tdi_info_test.cpp
)

target_compile_options(tdi_json_utest PRIVATE
  -Wno-error -Wno-unused-but-set-variable -Wno-unused-variable
  -Wno-unused-parameter -Wno-maybe-uninitialized
  "-DJSONDIR=\"${CMAKE_CURRENT_SOURCE_DIR}/tdi_json_files\""
)

target_link_libraries (tdi_json_utest
  gtest  # gtest_main
  gmock
  tdi
)

add_test(NAME TDI-JSON-UTEST
  COMMAND tdi_json_utest)
