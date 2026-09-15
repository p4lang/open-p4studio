# SPDX-FileCopyrightText: 2021 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.5)

project(libtdi VERSION 0.1 LANGUAGES CXX)

ENABLE_TESTING()
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

file(COPY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/
  PATTERN "*.doxy" EXCLUDE
  PATTERN "*.am" EXCLUDE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party)
include_directories(.)

if(STANDALONE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/target-syslibs/include)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/target-utils/include)
endif()
add_subdirectory(third-party)

set(C_CXX_FLAGS "-Wall -Wextra -Werror -Wshadow -Wpedantic -Wno-missing-field-initializers ")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,defs")

if(COVERAGE)
  set(C_CXX_FLAGS "${C_CXX_FLAGS} --coverage")
endif()
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${C_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_CXX_FLAGS}")


add_subdirectory(src)

#Building tdi doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/docs)
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)
  add_custom_target(doc-tdi
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating TDI Doxygen"
                    VERBATIM)
  set_target_properties(doc-tdi PROPERTIES EXCLUDE_FROM_ALL TRUE)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION
    share/doc/tdi OPTIONAL)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex DESTINATION
    share/doc/tdi OPTIONAL)

else()
  message(WARNING "Doxygen is needed to build the documentation. Please install doxygen.")
endif()
