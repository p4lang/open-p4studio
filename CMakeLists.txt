cmake_minimum_required(VERSION 3.5)
project(p4factory VERSION 9.13.4 LANGUAGES C CXX)
include(CheckCXXCompilerFlag)
include(ExternalProject)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    MESSAGE(STATUS "Enabling ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
  FORCE)
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "..." FORCE)
endif()
# Add the install prefix as system and lib includes.
include_directories(SYSTEM ${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

if(SDE_DEPENDENCIES)
  if (NOT IS_ABSOLUTE ${SDE_DEPENDENCIES})
    message(FATAL_ERROR "SDE_DEPENDENCIES should be an absolute path")
  endif ()
  set(CMAKE_FIND_ROOT_PATH ${SDE_DEPENDENCIES})
  file(RELATIVE_PATH SDE_DEPENDENCIES ${CMAKE_INSTALL_PREFIX} ${SDE_DEPENDENCIES})
else()
  set(SDE_DEPENDENCIES ".")
endif()

file(RELATIVE_PATH SDE ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR})
file(WRITE  "${CMAKE_INSTALL_PREFIX}/share/environment" "SDE_DEPENDENCIES=${SDE_DEPENDENCIES}\n")
file(APPEND "${CMAKE_INSTALL_PREFIX}/share/environment" "SDE=${SDE}\n")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/cmake/activate" DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

set(BF_PKG_DIR "pkgsrc")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}")

# Relative RPATH setup.
list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
set(CMAKE_SKIP_BUILD_RPATH FALSE)           # Keep RPATH in build tree
set(CMAKE_SKIP_INSTALL_RPATH FALSE)         # Keep RPATH when installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # Append linker paths to RPATH

# p4 specific options
option(TOFINO        "Architecture: Build P4 for tofino" OFF)
option(TOFINO2       "Architecture: Build P4 for tofino2" OFF)
option(TOFINO2M      "Architecture: Build P4 for tofino2m" OFF)

# thrift settings
option(THRIFT-DRIVER "Drivers: Build with support for Thrift in bf-drivers and bf-platforms" ON)
option(THRIFT-DIAGS  "BF-Diags: Build with support for Thrift in bf-diags" ON)
option(THRIFT-SWITCH "Switch: Build with support for Thrift in switch-p4-16" ON)

# driver options
option(GRPC          "Drivers: Build with support for GRPC/protobuf" ON)
option(BFRT          "Drivers: Build with BFRuntime support" ON)
option(BFRT-GENERIC-FLAGS "Drivers: Build BF-RT C API with generic flags support" ON)
option(PI            "Drivers: Build with tofino PI backend support" OFF)
option(P4RT          "Drivers: Build with P4Runtime server in bf_switchd support" OFF)
option(PER-PIPE-TBL-LOCK "Drivers: Build API with per pipe table lock enable" ON)
option(TDI           "Drivers: Build with TDI support" OFF)
option(KERNEL-MODULES "Drivers: Build kernel modules" ON)
option(SWITCH        "Switch: Build switch-p4-16 package" OFF)
option(SAI           "Switch: Build switch-p4-16 SAI implementation" ON)
option(BF-DIAGS      "BF-Diags: Build bf-diags package" OFF)

# tools
option(COVERAGE      "Build with gcov support" OFF)
option(TCMALLOC      "Build with gperftools" ON)
option(PROFILER      "Build with CPU profiler" OFF)
option(ASAN          "Build with ASAN" OFF)
option(LTO           "Build with LinkTimeOptimization" OFF)
option(FORCE32       "Force a 32bit compile on 64bit" OFF)
option(BF-PYTHON     "Build and enable bf-python CLI" ON)
option(BUILD_MODEL   "Build the Tofino model from source" OFF)

# global
option(ASIC          "Build for ASIC" OFF)
option(CPUVETH       "Use CPU eth port, applicable for model" ON)

# bf-platform specific
option(BSP           "BF-Platforms: Build with Intel reference platform code" OFF)
option(TCLONLY       "BF-Platforms: Build only tcl server in bf-platforms" OFF)
option(NEWPORT       "BF-Platforms: Build Newport Platform" OFF)
option(ACCTON-TF3    "BF-Platforms: Build Accton Tofino3 Platform" OFF)
option(ACCTON-DIAGS  "BF-Platforms: Build Accton Diags" OFF)
option(NEWPORT-DIAGS "BF-Platforms: Build Newport Diags" OFF)

# uncommon
option(TARGETSYS-DEBUG-MODE "Build target-syslibs in debug mode" OFF)

# Note 1: This option adds to p4flags "--p4runtime-files <p4_name>/<target>/<p4_name>.p4info.pb.txt --p4runtime-force-std-externs"
# Note 2: This option will install a compiler generated *p4info.pb.txt to install/share/<target>pd/<p4_name>
option(WITH-P4INFO "Generate P4Runtime compatible p4info file" OFF)

include(ConfigureChecks)

include(PythonDependencies)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (THRIFT-DRIVER OR THRIFT-SWITCH OR THRIFT-DIAGS)
  find_package(Thrift REQUIRED)
  if ( THRIFT_VERSION_STRING VERSION_LESS 0.14.0 )
    message(STATUS "\nThrift version (found ${THRIFT_VERSION_STRING}) is less than 0.14.0. Please update to avoid a known CVE in Thrift")
  endif()
endif()

#ptf-utils and bf-pktpy
set(PTF_PKG_DIR "ptf-modules")
add_subdirectory(${BF_PKG_DIR}/${PTF_PKG_DIR}/ptf-utils)
set(PYTHON_SITE ${SDE_PYTHON_DEPENDENCIES_DIR})
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/${BF_PKG_DIR}/${PTF_PKG_DIR}/ptf/ptf DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${BF_PKG_DIR}/${PTF_PKG_DIR}/ptf/src/ DESTINATION ${SDE_PYTHON_DEPENDENCIES_DIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${BF_PKG_DIR}/${PTF_PKG_DIR}/bf-pktpy/bf_pktpy DESTINATION ${SDE_PYTHON_DEPENDENCIES_DIR})

if(NOT EXTRA_CPPFLAGS)
  set(EXTRA_CPPFLAGS "" CACHE STRING "" FORCE)
else()
  message(STATUS "\nEXTRA_CPPFLAGS: ${EXTRA_CPPFLAGS}")
endif()

set(C_CXX_FLAGS "-Wall -Wextra -Wno-shadow -Wno-missing-field-initializers -DBF_SYS_LOG_FORMAT_CHECK ${EXTRA_CPPFLAGS}")

CHECK_CXX_COMPILER_FLAG(-Wmisleading-indentation COMPILER_SUPPORTS_MISLEADING_INDENTATION)
if (COMPILER_SUPPORTS_MISLEADING_INDENTATION)
  set(C_CXX_FLAGS "${C_CXX_FLAGS} -Wno-misleading-indentation")
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${C_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_CXX_FLAGS}")

# if lto is ready, build lto for all targets after this configruation
if(LTO)
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -flto=thin -fno-fat-lto-objects")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin -fno-fat-lto-objects")
endif()

if(COVERAGE)
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   --coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()
if(FORCE32)
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -m32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()
if(CMAKE_COMPILER_IS_GNUCC AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0))
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-error")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif()
if(ASAN)
  SET(TCMALLOC OFF CACHE BOOL "Use malloc from LIBC for ASAN" FORCE)
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fsanitize=address -fsanitize-recover=address -fsanitize=leak")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-recover=address -fsanitize=leak")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize-recover=address -fsanitize=leak")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address -fsanitize-recover=address -fsanitize=leak")
endif()
if(NEWPORT)
  SET(TOFINO OFF CACHE BOOL "Do not build Tofino platform for Newport" FORCE)
  SET(TOFINO3 OFF CACHE BOOL "Do not build Tofino3 platform for Newport" FORCE)
  if(NOT TOFINO2M)
    SET(TOFINO2 ON CACHE BOOL "Build for Tofino2 ASIC" FORCE)
  endif()
endif()
if(ACCTON-TF3)
  SET(TOFINO OFF CACHE BOOL "Do not build Tofino platform for Tofino3" FORCE)
  SET(TOFINO2 OFF CACHE BOOL "Do not build Tofino2 platform for Tofino3" FORCE)
  SET(TOFINO2M OFF CACHE BOOL "Do not build Tofino2m platform for Tofino3" FORCE)
  SET(TOFINO3 ON CACHE BOOL "Build for Tofino3 ASIC" FORCE)
endif()
if (NOT TOFINO AND NOT TOFINO2 AND NOT TOFINO2M AND NOT TOFINO3)
  set(TOFINO ON CACHE BOOL "Enable Tofino as default" FORCE)
  message(STATUS "\nTarget option missing. Using TOFINO\n")
endif()
if(ASIC)
  SET(CPUVETH OFF CACHE BOOL "CPUVETH is disabled for ASIC" FORCE)
  set(KERNEL-MODULES ON CACHE BOOL "Build kernel modules while using ASIC" FORCE)
endif()
if(TDI)
  SET(BFRT ON CACHE BOOL "BFRT is enabled for TDI" FORCE)
endif()
if (P4RT)
  set(PI ON CACHE BOOL "PI is enabled if P4RT is on" FORCE)
  set(GRPC ON CACHE BOOL "GRPC is enabled if P4RT is on" FORCE)
endif()

if (TARGETSYS-DEBUG-MODE)
  add_definitions(-DTARGET_SYS_LIBS_DEBUG_MODE)
endif()

if (STATIC-LINK-LIB)
  add_definitions(-DSTATIC_LINK_LIB)
endif()

# p4flags will be appended to internal compiler options
if(NOT P4FLAGS)
  set(P4FLAGS "" CACHE STRING "" FORCE)
else()
  message(STATUS "\nP4FLAGS: ${P4FLAGS}")
endif()

# p4ppflags will be appended to internal compiler options
if(NOT P4PPFLAGS)
  set(P4PPFLAGS "" CACHE STRING "" FORCE)
else()
  message(STATUS "\nP4PPFLAGS: ${P4PPFLAGS}")
endif()

include_directories(${BF_PKG_DIR})

#sys
message(STATUS "\nBegin target-syslibs setup")
include_directories(${BF_PKG_DIR}/target-syslibs/include)
add_subdirectory(${BF_PKG_DIR}/target-syslibs)

# bf-utils
message(STATUS "\nBegin bf-utils setup")
include_directories(${BF_PKG_DIR}/bf-utils/include)
add_subdirectory(${BF_PKG_DIR}/bf-utils)


#target-utils
message(STATUS "\nBegin target-utils setup")
include_directories(${BF_PKG_DIR}/target-utils/third-party/bigcode/include)
include_directories(${BF_PKG_DIR}/target-utils/third-party/klish)
include_directories(${BF_PKG_DIR}/target-utils/include)

add_subdirectory(${BF_PKG_DIR}/target-utils)

#drivers
message(STATUS "\nBegin bf-drivers setup")
include_directories(${BF_PKG_DIR}/bf-drivers)
include_directories(${BF_PKG_DIR}/bf-drivers/include)
add_subdirectory(${BF_PKG_DIR}/bf-drivers)

#compiler
# install bf-p4c
# FIXME: This setup is a little strange. Clean it up. Do we actually need all these variables?
ExternalProject_Add(bf-p4c
  DOWNLOAD_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
if (STANDALONE)
  set(P4C ${CMAKE_INSTALL_PREFIX}/bin/bf-p4c)
  set(P4C-GEN-BFRT-CONF ${CMAKE_INSTALL_PREFIX}/bin/p4c-gen-bfrt-conf)
  set(P4C-MANIFEST-CONFIG ${CMAKE_INSTALL_PREFIX}/bin/p4c-manifest-config)
else()
  # Use the build-local binaries if we are not building standalone.
  set(P4C ${CMAKE_BINARY_DIR}/${BF_PKG_DIR}/p4-compilers/p4c/bf-p4c)
  set(P4C-GEN-BFRT-CONF ${CMAKE_BINARY_DIR}/${BF_PKG_DIR}/p4-compilers/p4c/p4c-gen-bfrt-conf)
  set(P4C-MANIFEST-CONFIG ${CMAKE_BINARY_DIR}/${BF_PKG_DIR}/p4-compilers/p4c/p4c-manifest-config)
endif()
set(PDGEN ${CMAKE_INSTALL_PREFIX}/bin/generate_tofino_pd)
set(PDGEN_COMMAND ${PYTHON_COMMAND} ${PDGEN})
set(PDGENCLI ${CMAKE_INSTALL_PREFIX}/bin/gencli)
set(PDGENCLI_COMMAND ${PYTHON_COMMAND} ${PDGENCLI})
set(PDSPLIT ${CMAKE_INSTALL_PREFIX}/bin/split_pd_thrift.py)
set(PDSPLIT_COMMAND ${PYTHON_COMMAND} ${PDSPLIT})

add_subdirectory(${BF_PKG_DIR}/p4-compilers)

# model.
if(BUILD_MODEL)
  add_subdirectory(${BF_PKG_DIR}/tofino-model)
else()
  # install tofino-model by arch
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/${BF_PKG_DIR}/tofino-model/bin/tofino-model.x86_64.bin DESTINATION bin RENAME tofino-model)
  else()
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/${BF_PKG_DIR}/tofino-model/bin/tofino-model.i686.bin DESTINATION bin RENAME tofino-model)
  endif()
endif()

# install p4o
ExternalProject_Add(bf-p4o
  DOWNLOAD_COMMAND tar -xvf ${CMAKE_CURRENT_SOURCE_DIR}/${BF_PKG_DIR}/p4o/p4o-1.0.x86_64.tar.gz
      -C ${CMAKE_INSTALL_PREFIX} --strip-components=1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

if(ASIC AND BSP)
  #platform
  message(STATUS "\nBegin bf-platforms setup")
  include_directories(${BF_PKG_DIR}/bf-diags/api/include)
  add_subdirectory(${BF_PKG_DIR}/bf-platforms)
endif()

#diags
if(BF-DIAGS)
  message(STATUS "\nBegin bf-diags setup")
  add_subdirectory(${BF_PKG_DIR}/bf-diags)
endif()

#switch
if(SWITCH)
  message(STATUS "\nBegin switch-p4-16 setup")
  add_subdirectory(${BF_PKG_DIR}/switch-p4-16)
endif()

#p4-examples
message(STATUS "\nBegin p4-examples setup")
add_subdirectory(${BF_PKG_DIR}/p4-examples)

file(WRITE ${CMAKE_INSTALL_PREFIX}/share/VERSION ${PROJECT_VERSION})
