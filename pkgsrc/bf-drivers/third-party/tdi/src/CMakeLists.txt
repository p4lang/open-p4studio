# SPDX-FileCopyrightText: 2021 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TDI_SRCS
  tdi_init.cpp
  tdi_info.cpp
  tdi_target.cpp
  tdi_table.cpp
  tdi_table_data.cpp
  tdi_table_key.cpp
  tdi_learn.cpp
  #tdi_cjson.cpp
  #tdi_info_impl.cpp
  #tdi_table_info.cpp
  #tdi_utils.cpp
)

set(TDI_C_FRONTEND_SRCS
  c_frontend/tdi_init_c.cpp
  c_frontend/tdi_session_c.cpp
  c_frontend/tdi_table_c.cpp
  c_frontend/tdi_table_info_c.cpp
  c_frontend/tdi_table_data_c.cpp
  c_frontend/tdi_table_key_c.cpp
  c_frontend/tdi_table_attributes_c.cpp
  #c_frontend/tdi_table_operations_c.cpp
  #c_frontend/tdi_learn_c.cpp
  c_frontend/tdi_state_c.cpp
  c_frontend/tdi_info_c.cpp
)

add_library(tdi_o OBJECT ${TDI_SRCS} ${TDI_C_FRONTEND_SRCS})
add_library(tdi SHARED $<TARGET_OBJECTS:tdi_o>)

target_link_libraries(tdi PUBLIC target_utils target_sys tdi_json_parser)

add_subdirectory(arch/tna)
add_subdirectory(arch/pna)
add_subdirectory(arch/psa)
add_subdirectory(targets/dummy)
add_subdirectory(tdi_json_parser)
