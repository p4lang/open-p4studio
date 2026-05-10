# SPDX-FileCopyrightText: 2021 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

project(libtdi_tna VERSION 0.1 LANGUAGES CXX)

set(TDI_TNA_SRCS
  tna_init.cpp
  #tna_table_key.cpp
  tna_target.cpp
)

add_library(tdi_tna_o OBJECT ${TDI_TNA_SRCS})
add_library(tdi_tna SHARED $<TARGET_OBJECTS:tdi_tna_o>)
target_link_libraries(tdi_tna PUBLIC tdi)

