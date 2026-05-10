# SPDX-FileCopyrightText: 2021 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

project(libtdi_dummy VERSION 0.1 LANGUAGES CXX)

set(TDI_DUMMY_SRCS
  tdi_dummy_init.cpp
  c_frontend/tdi_dummy_init_c.cpp
)

add_library(tdi_dummy_o OBJECT ${TDI_DUMMY_SRCS})
add_library(tdi_dummy SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:tdi_dummy_o>)
target_link_libraries(tdi_dummy PUBLIC tdi_tna)

