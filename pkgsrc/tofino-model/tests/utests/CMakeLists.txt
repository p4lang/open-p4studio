option(model-utests "enable configuring unit test targets (default ON)" ON)
if (${model-utests})
    message("Configuring model unit tests targets")
else()
    message("Not configuring model unit tests targets")
    return()
endif ()
#
# By default all unit tests (including DV tests) are built.
# To build only a small subset of unit tests use -Dmodel-full-utests=OFF
# To build all unit tests except DV tests use -Dmodel-dv-utests=OFF
#
option(model-full-utests "enable building all unit tests (default ON)" ON)
option(model-dv-utests "enable building tofinoXX DV unit tests (default ON)" ON)
if (${model-full-utests})
    message("Building full suite of unit tests")
    if (${model-dv-utests})
        message("Building DV unit tests")
    else ()
        message("Not building DV unit tests")
    endif ()
else ()
    message("Building reduced suite of unit tests")
endif ()

add_compile_options(
        # disable -Werror in this directory
        -Wno-error
)

# Generic tests that do not need to be repeated per-chip...
set(GENERIC_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_asserts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_file_logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_json_loader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_log_buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_port_json_loader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_emplace_array.cpp
        )

# A small set of tests for reduced testing, common to all chips.
# New tests should probably NOT be added to this list.
set(COMMON_SUBSET_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_chip.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_overlaps.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_wrapper.cpp
        )

# Tests that are common to all chips.
# Add new tests here or in a chip specific list.
set(COMMON_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_action_output_hv_xbar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_actions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/input_xbar_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/input_xbar_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/cntrstack_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cntrstack_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/lpf_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lpf_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/meter_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/meter_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tcam_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tcam_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/test_bitvector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_caching_interval_map.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_capture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_cksum.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_crafter1.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_idle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_instr.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_ipb_common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_lfltr.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_lpf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_ltcam.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_mauhash.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_mau.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_meters.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_meter_sweep.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_mirror.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_p4_name_lookup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_packet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_phv.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pipe_object.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_port.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_queueing.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_registers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_rmt_debug.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_rmt_log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_rmt_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_salu_extracted.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_snapshot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_sps.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_stateful_alu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_stats.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_tcam2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_tcam3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_tcam.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_tcam_row_vh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_timer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_virt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv999.cpp
        )

# TofinoXX specific DV unit test files
set(TOFINO_DV_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dvXX.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv13.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv14.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv16.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv19.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv22.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv27.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv29.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv33.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv35.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv37.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv40.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv41.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv49.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv56.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv60.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv62.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv67.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv69.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv71.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv72.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv74.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv76.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv77.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv79.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv80.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv81.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv82.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv83.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv84.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv85.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv86.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv88.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv90.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv92.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv94.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv96.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv102.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv108.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv110.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv112.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv120.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv124.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv126.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv128.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv142.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv146.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv150.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv152.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv156.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv158.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv164.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv166.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv168.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv170.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv172.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv176.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv178.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv180.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv182.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv184.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv198.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv204.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv206.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv212.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_dv214.cpp

        # FIXME: Failing tests.
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_recipes.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv3.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv17.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv18.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv20.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv23.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv26.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv34.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv38.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv42.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv44.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv48.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv52.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv54.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv55.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv58.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv64.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv68.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/test_dv70.cpp

        # BROKEN_BY_TCAM_CRB_CHANGE
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv3.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv17.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv18.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv20.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv23.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv26.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv38.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv44.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv52.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv54.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv58.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv68.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv70.cpp

        # BROKEN_BY_SRAM_COL0_REMOVAL
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv34.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv42.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv48.cpp

        # BROKEN_BY_HYPERDEV_TRANSITION
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv55.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_test_dv64.cpp

        # not called/failing
        #${CMAKE_CURRENT_SOURCE_DIR}/test_dv31.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_dv32.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_dv224.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_dv234.cpp
        )

# TofinoXX specific unit test files
set(TOFINO_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_counters.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_deparse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_input_xbar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_ipb_tofino.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_epb_tofino.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parser_arbiter_tofino.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parse_tofino.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pktgen.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_rmt_packet_coordinator_tofinoxx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_stash.cpp
        )

# TofinoXX and JBay specific unit test files
set(TOFINOXX_JBAY_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_mac_counters.cpp
        # Note: in tests/Makefile.am this test was included for jbayXX and
        # tofinoB0 but not tofino, but the name suggests it should be ok for
        # tofino so here we include for tofinoXX
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parse_tofinoxx_jbay.cpp
        )

# JBay specific unit test files
set(JBAY_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_rmt_packet_coordinator_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_s2p_p2s_jbay.cpp
        )

# JBay and beyond unit test files
set(JBAY_EXTRA_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/pktgen_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pktgen_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/test_cksum_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_jbay_color_switchbox.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_jbay_counters.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_jbay_deparse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_jbay_extracted.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_jbay_homerow_color.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_jbay_pktgen.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_epb_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_ipb_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parser_arbiter_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_parse_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pkt_q.h
        ${CMAKE_CURRENT_SOURCE_DIR}/test_s2p_p2s.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_tm_sch_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_wrapper_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_egress_buf_jbay.cpp
        # previously missing on Jbay w.r.t. tofino...fail and/or ifdef'd out for jbay
        #${CMAKE_CURRENT_SOURCE_DIR}/test_event.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_input_xbar.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/test_stash.cpp
        )


###############################################################################
# supporting resources
###############################################################################

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_library(CRAFTER_LIB crafter HINTS /usr/intel/pkgs/libcrafter/1.0/lib REQUIRED)
message("Found crafter libs at ${CRAFTER_LIB}")
find_path(CRAFTER_INCLUDE_DIR crafter/Crafter.h HINTS /usr/intel/pkgs/libcrafter/1.0/include REQUIRED)
message("Found crafter includes at ${CRAFTER_INCLUDE_DIR}")
find_path(PCAP_INCLUDE_DIR pcap.h HINTS /usr/intel/pkgs/libcrafter/1.0/include REQUIRED)
message("Found pcap includes at ${PCAP_INCLUDE_DIR}")

set(COMMON_REQUIRED_UTEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util.cpp
        # TODO: are the following required for ALL unit tests or just DV?
        ${CMAKE_CURRENT_SOURCE_DIR}/tcam_row_vh_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tcam_row_vh_util.h
        ${CMAKE_CURRENT_SOURCE_DIR}/test_config.cpp
        )
# test_reader built as a separate lib so that it can be built with -j1 when
# necessary, see add_test_library_target()
set(TEST_READER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/action_hv_translator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/reader_actions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_reader.cpp
        )
# use -O3 flag when compiling test_reader.cpp
set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/test_reader.cpp
        PROPERTIES COMPILE_FLAGS -O3
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/extracted_tests
        COMMENT "Creating symlink to extracted tests in ${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND ln -sfF ${CMAKE_CURRENT_SOURCE_DIR}/extracted_tests ${CMAKE_CURRENT_BINARY_DIR}
        )
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources
        COMMENT "Creating symlink to utests resources in ${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND ln -sfF ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}
        )
add_custom_target(unit_test_prerequisites DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/extracted_tests
        )

###############################################################################
# Library for generic chip-independent tests
###############################################################################
add_library(generictest STATIC EXCLUDE_FROM_ALL)
target_sources(generictest PRIVATE ${GENERIC_UTEST_SOURCES})
target_include_directories(generictest PRIVATE ${PROJECT_SOURCE_DIR}/include)

###############################################################################
# Function to add a unit test lib for a chip type - implements the commands
# common to all chip types; chip-specific sources should be added to the target
# *after* calling this function to create the target
###############################################################################
function(add_test_library_target chip_type)
    # MODEL-695: MODEL-360: add a target to build only the test_reader for the
    # given chip type, e.g. a target "tofinotest_reader". This target is added
    # so that it can be built separately, for example using `make -j1
    # <chip_type>test_reader` when test_reader.cpp needs to be built in a
    # single process
    # Note: this is a STATIC lib rather than OBJECT because OBJECT libs can't
    # be linked to, at least not in older cmake.
    add_library(${chip_type}test_reader STATIC EXCLUDE_FROM_ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/test_reader.cpp)
    target_sources(${chip_type}test_reader PRIVATE ${TEST_READER_SOURCES})
    target_link_libraries(${chip_type}test_reader PRIVATE
            ${chip_type}_compiler_options
            ${chip_type}_includes)
    target_include_directories(${chip_type}test_reader PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            # TODO: MODEL-685: some utests files include <utests/xxx.h> so we
            # need this, but they could instead include "xxx.h"
            ${PROJECT_SOURCE_DIR}/tests
            )

    # Add a target for the given chip types test lib e.g. a target
    # "tofinotest"
    add_library(${chip_type}test STATIC EXCLUDE_FROM_ALL)
    # TODO: MODEL-688: why is there a utests include dir in the main project tree
    #   that necessitates ${chip_type}_includes here?
    target_link_libraries(${chip_type}test PRIVATE
            ${chip_type}_compiler_options
            ${chip_type}_includes
            ${chip_type}test_reader
            )
    target_include_directories(${chip_type}test PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            # TODO: MODEL-685: some utests files include <utests/xxx.h> so we
            # need this, but they could instead include "xxx.h"
            ${PROJECT_SOURCE_DIR}/tests
            )
endfunction()

###############################################################################
# Library targets for various chip types
###############################################################################
### libtofinotest target
add_test_library_target(tofino)
# Note: put new tests in appropriate *_UTEST_SOURCES list above, not here
target_sources(tofinotest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util_tofino.cpp
        ${COMMON_REQUIRED_UTEST_SOURCES}
        ${COMMON_SUBSET_UTEST_SOURCES}
        )
target_include_directories(tofinotest PUBLIC ${CRAFTER_INCLUDE_DIR} ${PCAP_INCLUDE_DIR})
if (${model-full-utests})
    target_sources(tofinotest PRIVATE
            ${COMMON_UTEST_SOURCES}
            ${TOFINO_UTEST_SOURCES}
            ${TOFINOXX_JBAY_UTEST_SOURCES}
            )
    if (${model-dv-utests})
        target_sources(tofinotest PRIVATE ${TOFINO_DV_UTEST_SOURCES})
    endif ()
endif ()


### libtofinoB0test target
#### lex, yacc required for tofinoB0test ####
add_custom_command(OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/lex.meter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/meterparseryacc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/meterparseryacc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/lex.lpf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lpfparseryacc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/lpfparseryacc.h
        DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/meterparser.lex
        ${CMAKE_CURRENT_SOURCE_DIR}/lpfparser.lex
        ${CMAKE_CURRENT_SOURCE_DIR}/meterparser.y
        ${CMAKE_CURRENT_SOURCE_DIR}/lpfparser.y
        COMMENT "Generating lex/yacc code"
        COMMAND lex --outfile=${CMAKE_CURRENT_SOURCE_DIR}/lex.meter.c ${CMAKE_CURRENT_SOURCE_DIR}/meterparser.lex
        COMMAND lex --outfile=${CMAKE_CURRENT_SOURCE_DIR}/lex.lpf.c ${CMAKE_CURRENT_SOURCE_DIR}/lpfparser.lex
        COMMAND yacc -d -p meter -o ${CMAKE_CURRENT_SOURCE_DIR}/meterparseryacc.c ${CMAKE_CURRENT_SOURCE_DIR}/meterparser.y
        COMMAND yacc -d -p lpf -o ${CMAKE_CURRENT_SOURCE_DIR}/lpfparseryacc.c ${CMAKE_CURRENT_SOURCE_DIR}/lpfparser.y
        )

add_test_library_target(tofinoB0)
# Note: put new tests in appropriate *_UTEST_SOURCES list above, not here
target_sources(tofinoB0test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util_tofino.cpp
        ${COMMON_REQUIRED_UTEST_SOURCES}
        ${COMMON_SUBSET_UTEST_SOURCES}
        )
target_include_directories(tofinoB0test PUBLIC ${CRAFTER_INCLUDE_DIR} ${PCAP_INCLUDE_DIR})
if (${model-full-utests})
    target_sources(tofinoB0test PRIVATE
            # Only build these lex/yacc files for TofinoB0
            ${CMAKE_CURRENT_SOURCE_DIR}/lex.meter.c
            ${CMAKE_CURRENT_SOURCE_DIR}/meterparseryacc.c
            ${CMAKE_CURRENT_SOURCE_DIR}/meterparseryacc.h
            ${CMAKE_CURRENT_SOURCE_DIR}/lex.lpf.c
            ${CMAKE_CURRENT_SOURCE_DIR}/lpfparseryacc.c
            ${CMAKE_CURRENT_SOURCE_DIR}/lpfparseryacc.h
            ${COMMON_UTEST_SOURCES}
            ${TOFINO_UTEST_SOURCES}
            ${TOFINOXX_JBAY_UTEST_SOURCES}
            )
    if (${model-dv-utests})
        target_sources(tofinoB0test PRIVATE ${TOFINO_DV_UTEST_SOURCES})
    endif ()
endif ()

### libjbaytest target
add_test_library_target(jbay)
# Note: put new tests in appropriate *_UTEST_SOURCES list above, not here
target_sources(jbaytest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util_jbay_shared.cpp
        ${COMMON_REQUIRED_UTEST_SOURCES}
        ${COMMON_SUBSET_UTEST_SOURCES}
        )
target_include_directories(jbaytest PUBLIC ${CRAFTER_INCLUDE_DIR} ${PCAP_INCLUDE_DIR})
if (${model-full-utests})
    target_sources(jbaytest PRIVATE
            ${COMMON_UTEST_SOURCES}
            ${TOFINOXX_JBAY_UTEST_SOURCES}
            ${JBAY_UTEST_SOURCES}
            ${JBAY_EXTRA_UTEST_SOURCES}
            )
endif ()


### libjbayB0test target
add_test_library_target(jbayB0)
# Note: put new tests in appropriate *_UTEST_SOURCES list above, not here
target_sources(jbayB0test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util_jbay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_util_jbay_shared.cpp
        ${COMMON_REQUIRED_UTEST_SOURCES}
        ${COMMON_SUBSET_UTEST_SOURCES}
        )
target_include_directories(jbayB0test PUBLIC ${CRAFTER_INCLUDE_DIR} ${PCAP_INCLUDE_DIR})
if (${model-full-utests})
    target_sources(jbayB0test PRIVATE
            ${COMMON_UTEST_SOURCES}
            ${TOFINOXX_JBAY_UTEST_SOURCES}
            ${JBAY_UTEST_SOURCES}
            ${JBAY_EXTRA_UTEST_SOURCES}
            )
endif ()






###############################################################################
# utests executable target
###############################################################################
add_executable(utests EXCLUDE_FROM_ALL
        gmock-gtest-all.cpp
        utest.cpp
        cmp_helper.cpp
        filesystem_helper.cpp
        main.cpp
        )
add_dependencies(utests unit_test_prerequisites)
target_include_directories(utests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/shared
        )
# Note: for each chip type, the chip specific tests lib is added to utests
# utests target to ensure dependency/ordering. We also want to force the whole
# lib archive to be linked so that gtests actually finds test classes so we add
# the lib again, with forced loading flags.
# Note: (for gcc at least) the order of the target_link_libraries is
# significant because we need the whole-archive test libs to be on linker
# command line *before* the model libs that they depend on.
# Note: the use of quotes around -Wl, ${FORCE_LOAD} etc seems necessary to
# correctly substitute the ${FORCE_LOAD} variable
message("Linking test libs with ${FORCE_LOAD}")
target_link_libraries(utests INTERFACE PRIVATE "-Wl,${FORCE_LOAD} $<TARGET_LINKER_FILE:generictest> -Wl,${NO_FORCE_LOAD}")
target_link_libraries(utests PRIVATE generictest)
if (${model-tofino})
    target_link_libraries(utests INTERFACE PRIVATE "-Wl,${FORCE_LOAD} $<TARGET_LINKER_FILE:tofinotest> -Wl,${NO_FORCE_LOAD}")
    target_link_libraries(utests PRIVATE tofinotest)
    target_link_libraries(utests PRIVATE tofinowrapper)
    target_link_libraries(utests PRIVATE tofino)
endif ()
if (${model-tofinoB0})
    target_link_libraries(utests INTERFACE PRIVATE "-Wl,${FORCE_LOAD} $<TARGET_LINKER_FILE:tofinoB0test> -Wl,${NO_FORCE_LOAD}")
    target_link_libraries(utests PRIVATE tofinoB0test)
    target_link_libraries(utests PRIVATE tofinoB0wrapper)
    target_link_libraries(utests PRIVATE tofinoB0)
endif ()
if (${model-jbay})
    target_link_libraries(utests INTERFACE PRIVATE "-Wl,${FORCE_LOAD} $<TARGET_LINKER_FILE:jbaytest> -Wl,${NO_FORCE_LOAD}")
    target_link_libraries(utests PRIVATE jbaytest)
    target_link_libraries(utests PRIVATE jbaywrapper)
    target_link_libraries(utests PRIVATE jbay)
endif ()
if (${model-jbayB0})
    target_link_libraries(utests INTERFACE PRIVATE "-Wl,${FORCE_LOAD} $<TARGET_LINKER_FILE:jbayB0test> -Wl,${NO_FORCE_LOAD}")
    target_link_libraries(utests PRIVATE jbayB0test)
    target_link_libraries(utests PRIVATE jbayB0wrapper)
    target_link_libraries(utests PRIVATE jbayB0)
endif ()
target_link_libraries(utests INTERFACE PRIVATE "-Wl,${FORCE_LOAD} $<TARGET_LINKER_FILE:modelrunnertest> -Wl,${NO_FORCE_LOAD}")
target_link_libraries(utests PRIVATE modelrunnertest)
target_link_libraries(utests PRIVATE modelrunner)

# The utests use Boost::Filesystem, which is not a header-only library.
# If your boost library is newer than your cmake, you will see the warning:
#     "Imported targets not available for Boost version ...".
# This is harmless, and is fixable by updating your cmake - do it!
find_package(Boost 1.54.0 REQUIRED COMPONENTS system filesystem)
message(STATUS "model-utests using boost libs: ${Boost_LIBRARIES}")
if (CMAKE_VERSION VERSION_LESS "3.13")
    link_directories(${Boost_LIBRARY_DIRS})
else()
    target_link_directories(utests PRIVATE ${Boost_LIBRARY_DIRS})
endif()

# Note: the order of libs is significant - dependents come before dependees
target_link_libraries(utests PRIVATE
        tofino_compiler_options
        modelcore
        ${Boost_LIBRARIES}
        ${CRAFTER_LIB}
        Threads::Threads
        )

###############################################################################
# check target to enable `make testscheck`; this mimics autotools check target
#
# Note: this target is not included in the `all` target
###############################################################################
add_custom_target(testscheck DEPENDS utests)


###############################################################################
# target to build test_reader lib for all the *configured* chip libs; this
# target is NOT included in the default targets (`all`)
###############################################################################
add_custom_target(test_reader_O3)
foreach (chip ${configured_chips})
    add_dependencies(test_reader_O3 ${chip}test_reader)
endforeach ()
