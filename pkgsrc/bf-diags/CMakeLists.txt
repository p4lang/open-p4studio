cmake_minimum_required(VERSION 3.5)
project(libdiags VERSION 0.1 LANGUAGES C CXX)

option(STANDALONE "Build outside of pkgsrc" OFF)
if (STANDALONE)
  set(CMAKE_CXX_STANDARD 17)
  set(CXX_STANDARD_REQUIRED ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_INSTALL_PREFIX}/include)
  link_directories(AFTER ${CMAKE_INSTALL_PREFIX}/lib)

  if(SDE_DEPENDENCIES)
    if (NOT IS_ABSOLUTE ${SDE_DEPENDENCIES})
      message(FATAL_ERROR "SDE_DEPENDENCIES should be an absolute path")
    endif ()
    set(CMAKE_FIND_ROOT_PATH ${SDE_DEPENDENCIES})
    file(RELATIVE_PATH SDE_DEPENDENCIES ${CMAKE_INSTALL_PREFIX} ${SDE_DEPENDENCIES})
  else()
    set(SDE_DEPENDENCIES ".")
  endif()

  add_custom_target(bf-p4c)
  add_custom_target(driver)
  find_program(P4C bf-p4c)
  message("P4C: " ${P4C})
  find_program(P4C-GEN-BFRT-CONF p4c-gen-bfrt-conf)
  message("P4C-GEN_BRFT-CONF: " ${P4C-GEN-BFRT-CONF})
  find_program(P4C-MANIFEST-CONFIG p4c-manifest-config)
  message("P4C-MANIFEST-CONFIG: " ${P4C-MANIFEST-CONFIG})
  find_program(PDGEN generate_tofino_pd)
  message("PDGEN: " ${PDGEN})
  find_program(PDGENCLI gencli)
  message("PDGENCLI: " ${PDGENCLI})
  find_program(PDSPLIT split_pd_thrift.py)
  message("PDSPLIT: " ${PDSPLIT})
endif() # STANDALONE

if (NOT PYTHON_EXECUTABLE)
  set(PYTHON_EXECUTABLE python3)
endif()

include(P4Build)
find_program(PDSPLIT split_pd_thrift.py REQUIRED)

include_directories(api/include)
include_directories(third-party/libcrafter/include)
add_subdirectory(third-party/libcrafter)


set(P4_NAME "diag")
set(P4_PATH "${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/diag.p4")
set(P4_LANG "p4-16")
set(P4FLAGS_INTERNAL "-Xp4c='--disable-power-check'")

set(DIAG-PROFILE "" CACHE STRING "BF-Diags: Diagnostic profile to build")
if (DIAG-PROFILE)
  set(P4PPFLAGS "-D${DIAG-PROFILE}")
  if (DIAG-PROFILE MATCHES "^DIAG_PHV_FLOP_MATCH.*")
    message("Forcing match dependency")
    set(P4FLAGS "-Xp4c=--force-match-dependency")
  elseif (DIAG-PROFILE STREQUAL "DIAG_PHV_FLOP_TEST=2")
    message("Disabling egress latency padding")
    set(NO_PADDING "'--disable-egress-latency-padding'")
    set(P4FLAGS "-Xassembler=${NO_PADDING}")
    #set(P4FLAGS "${P4FLAGS} -Xp4c=${NO_PADDING}")
  elseif (DIAG-PROFILE MATCHES "DIAG_PHV_FLOP_TEST=4")
    set(P4FLAGS "-Xp4c=--verbose")
  elseif (DIAG-PROFILE MATCHES "DIAG_PHV_FLOP_TEST=5")
    set(P4FLAGS "-Xp4c=--verbose")
    message("Forcing match dependency")
    set(P4FLAGS "${P4FLAGS} -Xp4c=--force-match-dependency")
  endif()

  if (DIAG-PROFILE MATCHES "DIAG_PHV_MOCHA_DARK")
    set(P4FLAGS "-Xp4c=--verbose")
    message("Forcing match dependency")
    set(P4FLAGS "${P4FLAGS} -Xp4c=--force-match-dependency")
    set(P4_PATH "${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/tf2_dark_mocha_diag.p4")
    set(PDFLAGS "--p4-prefix diag")
    # Add the TF1 PHV test defination so the control plane can be built with the
    # correct set of APIs/functionality enabled.
    add_definitions(-DDIAG_PHV_FLOP_TEST=1)
  endif()
  add_definitions(${P4PPFLAGS})
  add_definitions("-DDIAG_PROFILE=\"${DIAG-PROFILE}\"")
endif()

set(P4_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/defines.h
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/p4_table_sizes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/headers.p4
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/parser.p4
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/mau_bus_stress.p4
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/phv_datapath.p4
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/phv_flop_hdrs.p4
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/includes/phv_flop_test.p4
  ${CMAKE_CURRENT_SOURCE_DIR}/p4_16src/tf2_dark_mocha_diag.p4)

# p4_build_pd_target builds all PD artifacts using the P4Build.cmake module
# in p4factory.
# Use the p4_build_target function to build BFRT instead of PD when bfrt 
# becomes available

# No arch specified
if (NOT TOFINO AND NOT TOFINO2 AND NOT TOFINO2M)
  message(FATAL_ERROR "One and only one of the following architectures must be specified: TOFINO, TOFINO2, TOFINO2M")
endif()

# Tofino-1 & Tofino-2 specified
if (TOFINO AND (TOFINO2 OR TOFINO2M))
  message(FATAL_ERROR "One and only one of the following architectures must be specified: TOFINO, TOFINO2, TOFINO2M")
endif()

# Multiple Tofino-2 architectures specified
if (TOFINO2 AND TOFINO2M)
  message(FATAL_ERROR "One and only one of the following architectures must be specified: TOFINO, TOFINO2, TOFINO2M")
endif()

if (TOFINO)
  add_definitions(-DTOFINO1)
  set(P4_tofino_ARCHITECTURE "tna")
  p4_build_pd_target(${P4_NAME} ${P4_tofino_ARCHITECTURE} "tofino" ${P4_PATH})
endif()
if (TOFINO2)
  remove_definitions(-DTOFINO1)
  add_definitions(-DTOFINO2)
  set(P4_tofino2_ARCHITECTURE "t2na")
  p4_build_pd_target(${P4_NAME} ${P4_tofino2_ARCHITECTURE} "tofino2" ${P4_PATH})
endif()
if (TOFINO2M)
  remove_definitions(-DTOFINO1)
  add_definitions(-DTOFINO2)
  add_definitions(-DTOFINO2M)
  set(P4_tofino2_ARCHITECTURE "t2na")
  p4_build_pd_target(${P4_NAME} ${P4_tofino2_ARCHITECTURE} "tofino2m" ${P4_PATH})
endif()
if (TOFINO3) # postprocess
  remove_definitions(-DTOFINO1) # postprocess
  add_definitions(-DTOFINO3) # postprocess
  set(P4_tofino3_ARCHITECTURE "t2na") # postprocess
  p4_build_pd_target(${P4_NAME} ${P4_tofino3_ARCHITECTURE} "tofino3" ${P4_PATH}) # postprocess
endif() # postprocess

include_directories(${CMAKE_CURRENT_BINARY_DIR}/)
add_custom_target(diagspd DEPENDS
  $<$<BOOL:${TOFINO}>:${P4_NAME}-tofino>
  $<$<BOOL:${TOFINO2}>:${P4_NAME}-tofino2>
  $<$<BOOL:${TOFINO2M}>:${P4_NAME}-tofino2m>
  $<$<BOOL:${TOFINO3}>:${P4_NAME}-tofino3> # postprocess
)

if (THRIFT-DIAGS)
  add_subdirectory(thrift)
  include_directories(SYSTEM ${THRIFT_INCLUDE_DIR})
endif()

add_library(bfdiags SHARED
  api/src/diag_init.c
  api/src/diag_api.c
  api/src/diag_util.c
  api/src/diag_vlan.c
  api/src/diag_pd.c
  api/src/diag_pkt.c
  api/src/diag_ucli.c
  api/src/diag_create_pkt.cpp
  api/src/diag_pkt_database.cpp
  api/src/diag_pkt_eth_cpu.c
  api/src/diag_pkt_kernel.c
  $<TARGET_OBJECTS:crafter_o>
  $<$<BOOL:${THRIFT-DIAGS}>:$<TARGET_OBJECTS:diag_thrift_o>>
)
target_compile_definitions(bfdiags PRIVATE CMAKE_BUILD)
if (THRIFT-DIAGS)
  target_compile_definitions(bfdiags PRIVATE THRIFT_ENABLED)
endif()

add_dependencies(bfdiags diagspd)
target_link_libraries(bfdiags target_sys target_utils pcap)
set_source_files_properties(api/src/diag_ucli.c PROPERTIES COMPILE_FLAGS -Wno-format-extra-args)
set_source_files_properties(api/src/diag_pkt_database.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-but-set-variable)
set_source_files_properties(api/src/diag_pkt_eth_cpu.c PROPERTIES COMPILE_FLAGS -Wno-stringop-truncation)
if(CMAKE_COMPILER_IS_GNUCC AND (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0.0))
  set_source_files_properties(api/src/diag_create_pkt.cpp PROPERTIES COMPILE_FLAGS -Wno-deprecated-copy)
endif()
target_compile_options(bfdiags PRIVATE -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-shadow)

if (TOFINO)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/targets/tofino/${P4_NAME}.conf.in ${CMAKE_INSTALL_PREFIX}/share/p4/targets/tofino/${P4_NAME}.conf)
endif()
if (TOFINO2)
  set (TOFINO2_VARIANT "tofino2")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/targets/tofino2/${P4_NAME}.conf.in ${CMAKE_INSTALL_PREFIX}/share/p4/targets/tofino2/${P4_NAME}.conf)
endif()
if (TOFINO2M)
  set (TOFINO2_VARIANT "tofino2m")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/targets/tofino2/${P4_NAME}.conf.in ${CMAKE_INSTALL_PREFIX}/share/p4/targets/tofino2m/${P4_NAME}.conf)
endif()
if (TOFINO3)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/targets/tofino3/${P4_NAME}.conf.in ${CMAKE_INSTALL_PREFIX}/share/p4/targets/tofino3/${P4_NAME}.conf)
endif()
