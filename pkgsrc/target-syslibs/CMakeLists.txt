cmake_minimum_required(VERSION 3.5)
project(libtarget_sys VERSION 0.1 LANGUAGES C)

if(TARGETSYS-DEBUG-MODE)
    add_definitions(-DTARGET_SYS_LIBS_DEBUG_MODE)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ConfigureChecks.cmake)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party)

if (TCMALLOC)
  add_definitions(-DBF_SYS_LIBS_USE_TCMALLOC)
  if (PROFILER)
    set(TCMALLOC_FLAGS "--disable-shared")
  else()
    set(TCMALLOC_FLAGS "--disable-shared --enable-minimal")
  endif()
  set(C_FLAGS " -O3 -fPIC")
  set(CXX_FLAGS " -O3 -fPIC")
  if (FORCE32)
    set(C_FLAGS "${C_FLAGS} -m32")
    set(CXX_FLAGS "${CXX_FLAGS} -m32")
  endif()
  separate_arguments(TCMALLOC_FLAGS UNIX_COMMAND ${TCMALLOC_FLAGS})
  INCLUDE(ExternalProject)
  INCLUDE(ProcessorCount)
  ProcessorCount(N)
  ExternalProject_Add(build_tcmalloc
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third-party/gperftools
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/third-party/gperftools && ./autogen.sh
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/third-party/gperftools/configure
        --prefix=${CMAKE_INSTALL_PREFIX}
        ${TCMALLOC_FLAGS}
        CFLAGS=${C_FLAGS} CXXFLAGS=${CXX_FLAGS} > /dev/null
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/third-party/gperftools
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} -j${N} > /dev/null
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install > /dev/null
  )
  add_library(tcmalloclib UNKNOWN IMPORTED)
  if (PROFILER)
    set_property(TARGET tcmalloclib PROPERTY IMPORTED_LOCATION
      ${CMAKE_CURRENT_BINARY_DIR}/third-party/gperftools/.libs/libtcmalloc_and_profiler.a)
  else()
    set_property(TARGET tcmalloclib PROPERTY IMPORTED_LOCATION
      ${CMAKE_CURRENT_BINARY_DIR}/third-party/gperftools/.libs/libtcmalloc_minimal.a)
  endif()
  add_dependencies(tcmalloclib build_tcmalloc)
endif()

include_directories(include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/libev/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/zlog/include)
add_subdirectory(src/bf_sal)
add_subdirectory(third-party/libev)

function(buildZlog)
  include_directories(zlog/src/)
  add_library(zlog_o OBJECT
    third-party/zlog/src/buf.c
    third-party/zlog/src/category.c
    third-party/zlog/src/category_table.c
    third-party/zlog/src/conf.c
    third-party/zlog/src/event.c
    third-party/zlog/src/format.c
    third-party/zlog/src/level.c
    third-party/zlog/src/level_list.c
    third-party/zlog/src/mdc.c
    third-party/zlog/src/record.c
    third-party/zlog/src/record_table.c
    third-party/zlog/src/rotater.c
    third-party/zlog/src/rule.c
    third-party/zlog/src/spec.c
    third-party/zlog/src/thread.c
    third-party/zlog/src/zc_arraylist.c
    third-party/zlog/src/zc_hashtable.c
    third-party/zlog/src/zc_profile.c
    third-party/zlog/src/zc_util.c
    third-party/zlog/src/zlog.c)

  target_compile_options(zlog_o PRIVATE -Wno-shadow -Wno-sign-compare
    -Wno-unused-parameter -Wno-error=cpp -Wno-error=parentheses)
  add_library(zlog SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:zlog_o>)
  #  install(FILES  third-party/zlog-cfg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/targetsys)
  file(COPY third-party/zlog/src/ DESTINATION
    ${CMAKE_INSTALL_PREFIX}/include/target-sys/zlog/ FILES_MATCHING PATTERN *.h)
endfunction()

buildZlog()

add_library(target_sys SHARED
  $<TARGET_OBJECTS:zlog_o>
  $<TARGET_OBJECTS:ev_o>
  $<TARGET_OBJECTS:bf_sal_o>
)
if (TCMALLOC)
  if (NOT ASAN)
    if (PROFILER)
      target_link_libraries(target_sys PUBLIC tcmalloclib stdc++ pthread unwind)
    else()
      target_link_libraries(target_sys PUBLIC tcmalloclib stdc++ pthread)
    endif()
  endif()
else()
target_link_libraries(target_sys PUBLIC pthread)
endif()

file(COPY include/target-sys DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  PATTERN "*.am" EXCLUDE)
