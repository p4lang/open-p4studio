option(model-runner "build the model runner executable (default OFF)" OFF)
# MODEL-597: By default the model runner is fully statically linked. For partial
# static linking we are more selective; the goal of partial static linking is
# to statically link as much as possible without including certain libs e.g.
# gettext (via libc) and cli.
option(runner-static "statically link runner (default ON)" ON)
option(runner-static-partial "statically link runner with some but not all libs; overrides runner-static option (default OFF)" OFF)

# if -Dmodel-internal OR BFN_INTERNAL set in enviroment, compile with -DBFN_INTERNAL
if (${model-internal} OR DEFINED ENV{BFN_INTERNAL})
   add_definitions(-DBFN_INTERNAL)
endif ()

###############################################################################
# target for libmodelrunner.a - builds components of runner that do not
# have external dependencies on libcli, lipcap etc
#
# Note: this lib is built regardless of the model-runner option so that it can
# be used in utests; this target is included in the default targets (`all`) and
# so will be built by `make`
###############################################################################
add_library(modelrunner STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/os_privs.cpp
        )

target_include_directories(modelrunner PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

# add runner utests
add_subdirectory(tests/utests)

if(NOT ${model-runner})
    message("Model runner not requested in configuration")
    return()
endif()

###############################################################################
# The following is conditional on model-runner being set to ON...
###############################################################################
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# find library for lib_name with specific suffix and store in var_name; similar
# to built-in find_library but with additional suffix argument
function(find_specific_library var_name lib_name suffix)
    # clear cache so that find_library always looks for library
    unset(${var_name} CACHE)
    # set lib suffix matcher to static libs
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${suffix})
    find_library(${var_name} ${lib_name})
    # reset lib suffix matcher to default
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endfunction()

if (${runner-static-partial})
    find_specific_library(CLI_LIB cli ".so")
    find_specific_library(PCAP_LIB pcap ".a")
    find_specific_library(CRYPT_LIB crypt ".a")
elseif (${runner-static})
    find_specific_library(CLI_LIB cli ".a")
    find_specific_library(PCAP_LIB pcap ".a")
    find_specific_library(CRYPT_LIB crypt ".a")
else()
    find_library(CLI_LIB cli)
    find_library(PCAP_LIB pcap)
    find_library(CRYPT_LIB crypt)
endif()

message("Found ${CRYPT_LIB}")
message("Found ${PCAP_LIB}")

set(configure_model_runner ON)
if(NOT CLI_LIB)
    message("Cannot find cli lib")
    set(configure_model_runner OFF)
else()
    message("Found ${CLI_LIB}")
endif()
if(NOT PCAP_LIB)
    message("Cannot find pcap lib")
    set(configure_model_runner OFF)
else()
    message("Found ${PCAP_LIB}")
endif()
if(NOT CRYPT_LIB)
    message("Cannot find crypt lib")
    set(configure_model_runner OFF)
else()
    message("Found ${CRYPT_LIB}")
endif()

if (NOT ${configure_model_runner})
    message("Missing required libs for runner, unable to configure model runner targets")
else ()
    ###############################################################################
    # target for libmodelrunner_extra.a - builds components of runner that depend on
    # platform specific libs but do not have external dependency on libmodel.a
    # from bf-drivers
    ###############################################################################
    message("Configuring model runner library targets")
    add_library(modelrunner_extra STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/bmi_interface.c
            ${CMAKE_CURRENT_SOURCE_DIR}/bmi_port.c
            ${CMAKE_CURRENT_SOURCE_DIR}/cli.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/portmanager.c
            )
    target_include_directories(modelrunner_extra PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/include/shared
            ${PROJECT_SOURCE_DIR}/include/model_core
            )
    target_compile_options(modelrunner_extra PRIVATE
            -DLIBPCAP_USE_FIX
            # avoid false positive re unused pkt_rx callback function...
            -Wno-unused-function
            )

    # libmodel is provided by bf-drivers so we need to know where that is installed
    # via ${CMAKE_INSTALL_PREFIX} option
    # Note: this lib may not exist at time cmake is run (e.g. when building the
    # model as part of p4factory) so we proceed unconditionally
    set(BF_DRIVER_MODEL_LIB ${CMAKE_INSTALL_PREFIX}/lib/libmodel.a)
    if(EXISTS ${BF_DRIVER_MODEL_LIB})
        message("Found ${BF_DRIVER_MODEL_LIB}")
    else()
        message("Warning: ${BF_DRIVER_MODEL_LIB} not found but will be "
                "required to build model runner")
    endif()
    ###############################################################################
    # target for the tofino-model.debug executable - this depends on libmodel.a
    # from bf-drivers
    ###############################################################################
    message("Configuring model runner executable targets")
    add_executable(tofino-model.debug
            ${CMAKE_CURRENT_SOURCE_DIR}/lld_intf.c
            ${CMAKE_CURRENT_SOURCE_DIR}/simulator_intf.c
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
            )

    target_link_libraries(tofino-model.debug PRIVATE modelrunner_extra)
    target_link_libraries(tofino-model.debug PRIVATE modelrunner)
    target_link_libraries(tofino-model.debug PRIVATE configured_chips_compiler_options)
    # Note: libmodelcore.a is added to link lib list twice, before and
    # after the chip libs
    target_link_libraries(tofino-model.debug PRIVATE modelcore)
    target_link_libraries(tofino-model.debug PRIVATE ${configured_chips})
    target_link_libraries(tofino-model.debug PRIVATE modelcore)

    target_include_directories(tofino-model.debug PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/include/shared
            ${PROJECT_SOURCE_DIR}/include/model_core
            ${CMAKE_INSTALL_PREFIX}/include
            )

    if (${runner-static-partial})
        target_link_libraries(tofino-model.debug PRIVATE
                -static-libstdc++ -static-libgcc
                ${CLI_LIB}
                "-Wl,-Bstatic"
                ${CRYPT_LIB}
                ${PCAP_LIB}
                "-Wl,-Bdynamic"
                ${BF_DRIVER_MODEL_LIB}
                Threads::Threads
                )
    elseif (${runner-static})
        target_link_libraries(tofino-model.debug PRIVATE
                -static
                "-Wl,${WHOLE_ARCHIVE} -lpthread -Wl,${NO_WHOLE_ARCHIVE}"
                ${CLI_LIB}
                ${CRYPT_LIB}
                ${PCAP_LIB}
                ${BF_DRIVER_MODEL_LIB}
                Threads::Threads
                )
    else ()
        target_link_libraries(tofino-model.debug PRIVATE
                ${CLI_LIB}
                ${CRYPT_LIB}
                ${PCAP_LIB}
                ${BF_DRIVER_MODEL_LIB}
                Threads::Threads
                )
    endif ()

    ###############################################################################
    # Target for tofino-model executable (stripped of symbols)
    ###############################################################################
    # Use objcopy tool to create a stripped binary called tofino-model.
    # (This stripped binary will be the *only* binary supplied to customers)
    #
    # Stripping process involves:
    # 1. Stripping all debug tokens,
    # 2. Linking stripped binary to tofino-model.debug binary so debugging still possible
    #    (on systems where tofino-model.debug binary made available),
    # 3. Stripping tokens listed in strip-symbols.txt file (typically CSR tokens).
    #
    add_custom_target(runner
            ALL
            COMMENT "Generating symbol-stripped tofino-model executable from tofino-model.debug"
            DEPENDS $<TARGET_FILE:tofino-model.debug>
            COMMAND objcopy
            --strip-debug
            --add-gnu-debuglink=$<TARGET_FILE:tofino-model.debug>
            --wildcard --strip-symbols=${CMAKE_CURRENT_SOURCE_DIR}/strip-symbols.txt
            $<TARGET_FILE:tofino-model.debug>
            tofino-model
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
    # install executables to ${CMAKE_INSTALL_PREFIX}/bin, which
    # defaults to /bf/install/bin
    install(TARGETS tofino-model.debug DESTINATION bin)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/tofino-model DESTINATION bin)
endif ()
