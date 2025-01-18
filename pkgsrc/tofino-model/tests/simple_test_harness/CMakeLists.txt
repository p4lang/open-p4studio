option(simple-test-harness "enable simple test harness" ON)
if (NOT ${simple-test-harness})
    message("Not configuring model simple test harness targets")
    return()
else ()
    message("Configuring model simple test harness targets")
endif ()

add_compile_options(
        # disable -Werror in this directory
        -Wno-error
)

# Note: the legacy autotools configure.ac had pcap as a requirement for simple
# test harness but it does not appear to be used, so omitting it here
# find_library(PCAP_LIB pcap)
find_package(Threads REQUIRED)

set(TEST_HARNESS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/bfrt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/bson.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/input.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/json.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/p4name.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/packet.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/script.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sleep.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/table_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/write.cpp
        )


###############################################################################
# top-level target to build test harness executables for all configured chip
# types using cmake simple_test_harness
###############################################################################
add_custom_target(simple_test_harness)


###############################################################################
# Function to add a target to build a test_harness_<name> executable for a
# given <chip_type>.
###############################################################################
function(add_test_harness name chip_type)
    set(target_name ${name}_test_harness)
    add_executable(${target_name} ${TEST_HARNESS_SOURCES})
    target_link_libraries(${target_name} PRIVATE
            ${chip_type}_includes
            ${chip_type}_compiler_options
            # Note: libmodelcore.a is added to link lib list twice, before and
            # after the chip lib
            modelcore
            # Note: modelcore uses symbols for all configured chip types so we
            # need to provide the libs for all configured chip types even
            # though this test harness target is for just one chip type.
            ${configured_chips}
            modelcore
            "-Wl,${WHOLE_ARCHIVE} -lpthread -Wl,${NO_WHOLE_ARCHIVE}"
            dl
            Threads::Threads
            )
    add_dependencies(simple_test_harness ${target_name})
endfunction()

###############################################################################
# Test harness executable target for each chip type
###############################################################################
if (${model-tofino} AND ${model-tofinoB0})
    # If both TOFINO and TOFINOB0 are enabled, we build tofino as tofinoA0
    add_test_harness(tofinoA0 tofino)
elseif (${model-tofino})
    # FIXME -- this should also be tofinoB0 -- should need to explicitly use tofinoA0 to get it
    add_test_harness(tofino tofino)
endif ()

if (${model-tofinoB0})
    add_test_harness(tofino tofinoB0)
endif ()

if (${model-jbay} AND ${model-jbayB0})
    # If both JBAY and JBAYB0 are enabled, we build jbay as jbayA0
    add_test_harness(jbayA0 jbay)
elseif (${model-jbay})
    # FIXME -- this should also be jbayB0 -- should need to explicitly use jbayA0 to get it
    add_test_harness(jbay jbay)
endif ()

if (${model-jbayB0})
    add_test_harness(jbay jbayB0)
endif ()

