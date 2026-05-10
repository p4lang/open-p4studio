# SPDX-FileCopyrightText: 2022 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

project(libtdi_json_parser VERSION 0.1 LANGUAGES CXX)

set(TDI_JSON_PARSING_SRCS
  tdi_cjson.cpp
  tdi_info_parser.cpp
  tdi_learn_info.cpp
  tdi_table_info.cpp
)

add_library(tdi_json_parser_o OBJECT ${TDI_JSON_PARSING_SRCS})
add_library(tdi_json_parser SHARED $<TARGET_OBJECTS:tdi_json_parser_o>)
target_link_libraries(tdi_json_parser PUBLIC target_sys target_utils)

if(TDI_GTEST)
  add_subdirectory(tests)
endif()
