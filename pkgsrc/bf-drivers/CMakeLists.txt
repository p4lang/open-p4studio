cmake_minimum_required(VERSION 3.5)
project(libdriver VERSION 0.1 LANGUAGES C CXX)

include(PythonDependencies)

option(STANDALONE "Build outside of pkgsrc" OFF)

if (STANDALONE)
  option(FORCE32    "Force a 32bit compile on 64bit" OFF)
  option(ASAN       "Build with ASAN" OFF)
  option(COVERAGE   "Build with gcov" OFF)
  option(ASIC       "Build for ASIC" OFF)

  option(GRPC       "Build with support for GRPC/protobuf" OFF)
  option(THRIFT-DRIVER "Build with support for Thrift" ON)
  option(BF-PYTHON  "Build and enable bf-python CLI" OFF)
  option(BFRT       "Build with BFRuntime support" ON)
  option(TDI        "Build with TDI support" OFF)
  option(BFRT-GO    "Build with bfruntime Golang gRPC and protobuf bindings" OFF)
  option(PI         "Build with tofino PI backend support" OFF)
  option(P4RT       "Build with P4Runtime support" OFF)
  option(BFRT-GENERIC-FLAGS "Build BF-RT C API with generic flags support" ON)
  option(PER-PIPE-TBL-LOCK "Build API with per pipe table lock enable" ON)

  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_STANDARD 17)
  set(CXX_STANDARD_REQUIRED ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")

  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ConfigureChecks.cmake)

  set(C_CXX_FLAGS "-Wall -Wextra -Werror -Wshadow -Wpedantic -Wno-missing-field-initializers -Wno-misleading-indentation -DBF_SYS_LOG_FORMAT_CHECK")
  if(COVERAGE)
    set(C_CXX_FLAGS "${C_CXX_FLAGS} --coverage")
  endif()
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${C_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_CXX_FLAGS}")
  if(FORCE32)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  endif()

  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
  include_directories(${CMAKE_INSTALL_PREFIX}/include)

  if (NOT PYTHON_EXECUTABLE)
    set(PYTHON_EXECUTABLE python3)
  endif()

endif() # STANDALONE

if (THRIFT-DRIVER)
  find_package(Thrift REQUIRED)
endif()

if(TDI)
  SET(TDI-PYTHON ON CACHE BOOL "Build and enable tdi-python CLI" FORCE)
endif()

message(STATUS "\nCMAKE_C_FLAGS : ${CMAKE_C_FLAGS}")
string(REGEX MATCH "DEVICE_IS_EMULATOR_SERDES_56G" emulatorflag ${CMAKE_C_FLAGS})
if(${emulatorflag} MATCHES "DEVICE_IS_EMULATOR_SERDES_56G")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEVICE_IS_EMULATOR=2")
  message(STATUS "\nCMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
endif()



link_directories(AFTER ${CMAKE_INSTALL_PREFIX}/lib)

if(BFRT-GENERIC-FLAGS)
  set(GENERIC_FLAGS   "-DBFRT_GENERIC_FLAGS")
endif()

if(PER-PIPE-TBL-LOCK)
  set(PER_PIPE_TBL_LOCK   "-DPIPE_MGR_PER_PIPE_TABLE_LOCK_ENABLE")
endif()

# save the top-level cflags for libmodel which doesn't require DRV_CFLAGS
set(OLD_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# split all of these flags into per target definitions if possible
set(DRV_UCLI_CFLAGS "-DUCLI_CONFIG_INCLUDE_ELS_LOOP=1 -DLLDLIB_CONFIG_INCLUDE_UCLI=1 -DPORT_MGR_CONFIG_INCLUDE_UCLI=1 -DDVM_CONFIG_INCLUDE_UCLI=1 -DPIPE_MGR_CONFIG_INCLUDE_UCLI=1 -DMC_MGR_CONFIG_INCLUDE_UCLI=1 -DTRAFFIC_MGR_CONFIG_INCLUDE_UCLI=1 -DDEVDIAG_CONFIG_INCLUDE_UCLI=1")
set(DRV_THRD_CFLAGS "-DPIPE_MGR_THREADS -DPIPE_MGR_INLINE_DR_SERVICE -DMC_MGR_THREADS -DMC_MGR_INLINE_DR_SERVICE -DTM_MT_SAFE")
set(DRV_MISC_CFLAGS "-DTARGET_IS_LLD -DDEBUG_MODE=0 -DHARLYN_DEBUG_MODE=0 ${GENERIC_FLAGS}")
set(DRV_MISC_CFLAGS "${DRV_MISC_CFLAGS} ${PER_PIPE_TBL_LOCK}")
set(DRV_CFLAGS "${DRV_UCLI_CFLAGS} ${DRV_THRD_CFLAGS} ${DRV_MISC_CFLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DRV_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DRV_CFLAGS}")

if (PI)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPI_ENABLED")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPI_ENABLED")
endif()
if (P4RT)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DP4RT_ENABLED")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DP4RT_ENABLED")
endif()
if (BFRT)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBFRT_ENABLED")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBFRT_ENABLED")
endif()
if(TDI)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTDI_ENABLED")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTDI_ENABLED")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/tdi/include)
endif()
if(GRPC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGRPC_ENABLED")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGRPC_ENABLED")
endif()
if (THRIFT-DRIVER)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTHRIFT_ENABLED")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTHRIFT_ENABLED")
endif()

set(BF_PYTHON_VER "${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")
set(TDI_PYTHON_VER ${BF_PYTHON_VER})

add_subdirectory(third-party)
add_subdirectory(src)
if (THRIFT-DRIVER)
  add_subdirectory(pdfixed_thrift)
endif()
if (NOT STANDALONE)
  add_subdirectory(bf_switchd)
endif()

add_subdirectory(pd_api_gen)

#kernel driver
if (KERNEL-MODULES)
  add_subdirectory(kdrv)
endif()

file(COPY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  PATTERN "*.doxy" EXCLUDE
  PATTERN "*.am" EXCLUDE)

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kdrv/bf_kdrv_mod_load DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kdrv/bf_kdrv_mod_unload DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kdrv/bf_knet_mod_load DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kdrv/bf_knet_mod_unload DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kdrv/bf_kpkt_mod_load DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kdrv/bf_kpkt_mod_unload DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/tools/bf_model_driver_remote DESTINATION bin)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tools/bf_switchd_dev_status.py DESTINATION ${SDE_PYTHON_DEPENDENCIES_DIR}/p4testutils)

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(srcdir ${CMAKE_CURRENT_SOURCE_DIR}/doc)
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  if(BFRT)
    add_custom_target(doc-driver
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                    DEPENDS doc-driver-bfrt doc-driver-bfrt-fixed
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating bf-drivers top level Doxygen"
                    VERBATIM)
    # To resolve external doc links from BFRT FF to TM APis
    add_dependencies(doc-driver-bfrt-fixed doc-driver-traffic-mgr)
  else()
    add_custom_target(doc-driver
                    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating bf-drivers top level Doxygen"
                    VERBATIM)
  endif()

  set_target_properties(doc-driver PROPERTIES EXCLUDE_FROM_ALL TRUE)

  find_package(Doxypy QUIET)
  if(DOXYPY_FOUND)
      if(BFRT)
        add_dependencies(doc-driver doc-driver-bfrt-python-grpc)
      endif()
  endif()

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION
    share/doc/bf-drivers OPTIONAL)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex DESTINATION
    share/doc/bf-drivers OPTIONAL)

  # bf-drivers module specific documentation
  add_custom_target(dox-driver
                  DEPENDS doc-driver-pdfixed doc-driver-pipe-mgr doc-driver-traffic-mgr doc-driver-bf-pm doc-driver-pkt-mgr doc-driver-mc-mgr doc-driver-lld doc-driver-dvm doc-driver-dru-sim doc-driver-diag doc-driver-ctx-json
                  COMMENT "Generating bf-drivers per module doxygen"
                  VERBATIM)
  if(BFRT)
      add_dependencies(dox-driver doc-driver-bfrt doc-driver-bfrt-fixed)
      if(DOXYPY_FOUND)
        add_dependencies(dox-driver doc-driver-bfrt-python-grpc)
      endif()
  endif()

else()
  message(WARNING "Doxygen is needed to build the documentation.")
endif()


