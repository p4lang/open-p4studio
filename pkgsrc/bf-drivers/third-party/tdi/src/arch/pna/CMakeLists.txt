# SPDX-FileCopyrightText: 2022 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

project(libtdi_pna VERSION 0.1 LANGUAGES CXX)

set(TDI_PNA_SRCS
  pna_init.cpp
  pna_target.cpp
)

add_library(tdi_pna_o OBJECT ${TDI_PNA_SRCS})
add_library(tdi_pna SHARED $<TARGET_OBJECTS:tdi_pna_o>)
target_link_libraries(tdi_pna PUBLIC tdi)

