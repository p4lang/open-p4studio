cmake_minimum_required(VERSION 3.5)

project(libtarget_utils VERSION 0.1 LANGUAGES C)


set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ConfigureChecks.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=unused-parameter")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# Including target-syslibs only for standalone build of target-utils
if (STANDALONE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/target-syslibs/include)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/bigcode/include/target_utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/cJSON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/klish/target_utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/judy-1.0.5/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/tommyds/tommyds)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/xxHash)


add_subdirectory(src)
add_subdirectory(third-party)

add_library(target_utils SHARED
  $<TARGET_OBJECTS:target_sysutil_o>
  $<TARGET_OBJECTS:xxhash_o>
  $<TARGET_OBJECTS:tommyds_o>
  $<TARGET_OBJECTS:JudyCommon>
  $<TARGET_OBJECTS:JudyL>
  $<TARGET_OBJECTS:Judy1>
  $<TARGET_OBJECTS:JudySL>
  $<TARGET_OBJECTS:JudyHS>
)

# When linking cjson, we want to ensure that the whole archive is added
# to the target_utils library and the symbols are forced to always be in.
# Switching the whole-archive option after linking cjson is important so
# that other archives and libraries don't start using this option
target_link_libraries(target_utils PUBLIC "-Wl,--whole-archive" cjson "-Wl,--no-whole-archive")

file(COPY include/target-utils DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  PATTERN "*.doxy" EXCLUDE
  PATTERN "*.am" EXCLUDE)

