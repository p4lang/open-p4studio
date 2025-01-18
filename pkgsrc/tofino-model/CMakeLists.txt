# Targets
# =======
#
# Model targets in this file:
#   model-autotools
#     Builds utests executable for chip type tofinoB0 using the legacy
#     autotools process.
#     Depends on: N/A
#   generate_version_file
#     Creates a /include/model_core/model-version.h file which contains
#     information about the current git status
#     Depends on: N/A
#   modelcore
#     Builds libmodelcore.a
#     Depends on: generate_version_file
#   <chip>
#     Builds lib<chip>.a where <chip> is in [tofino, tofinoB0, jbay, jbayB0]
#     Depends on: N/A
#
# Model targets in tests/wrapper/CMakeLists.txt:
#   tofinowrapper
#     Builds libtofinowrapper.a
#     Depends on: N/A
#
# Model targets in tests/utests/CMakeLists.txt:
#   unit_test_prerequisites:
#     Creates symlinks from build dir to resources required for unit testing
#     Depends on: N/A
#   generictest:
#     Builds libgenerictest.a containing generic unit tests
#     Depends on: N/A
#   <chip>test:
#     Builds lib<chip>test.a containing unit tests for chip type
#     Depends on: N/A
#   utests:
#     Builds utests executable which runs ALL unit tests
#     Depends on: unit_test_prerequisites
#                 generictest
#                 <chip>test for each configured chip
#                 tofinowrapper
#                 modelcore
#                 <chip> for each configured chip
#
#
# Project options
# ===============
#
# The following model-related options are supported. Each can be specified
# as ON or OFF using a -D option on the cmake command line.
#
# model-internal - build with BFN-internal extra code (default is OFF)
#
# model-tofino   - build for tofino chip type (default is OFF)
# model-tofinoB0 - build for tofinoB0 chip type (default is OFF)
# model-jbay     - build for jbay chip type (default is OFF)
# model-jbayB0   - build for jbayB0 chip type (default is OFF)
#
# If none of the model-<chip> options are specified then the tofino chip type
# will be built.
#
# precompiled-headers - enable precompiled headers for chip libs (default is ON)
#
# model-runner build the model runner (a.k.a. tofino-model) executable (default is OFF)
#
# CMAKE_INSTALL_PREFIX - (built-in) specifies the directory under which
#   bf-drivers header and lib are located for building the tofino-model
#   executable (default is /usr/local) e.g. -DCMAKE_INSTALL_PREFIX=/bf/install
#
# runner-static - specifies that the model runner executable should
#   statically linked with all libraries
# runner-static-partial - specifies that the model runner executable should
#   statically linked with some but not all libraries (as opposed to the
#   default of fully statically linked); overrides runner-static.
#
# model-utests - enable configuring unit test targets (default is ON)
# model-full-utests - build all unit tests; setting this to OFF disables most
#   unit tests including DV tests (default is ON)
# model-dv-utests - build DV unit tests; setting this to OFF disables DV
#   unit tests (default is ON)
#
# simple-test-harness - enable simple test harness (default is ON)
# model-wrapper - enable model wrappers (default is ON)
#
# CPPFLAGS - flags to be added to CMAKE_CXX_FLAGS (same effect as CXXFLAGS)
# CXXFLAGS - flags to be added to CMAKE_CXX_FLAGS (same effect as CPPFLAGS)
# CFLAGS - flags to be added to CMAKE_C_FLAGS
#
# Notes
# =====
# - Most source files specified separately from the targets that build them
# using a number of variables named *_SOURCES. These variables each expand to a
# list of files. Each *_SOURCES variable may be used to specify sources for one
# or more target. This allows source files shared by more than one chip type to
# be listed just once in this file.
#
# - Some of the targets use the EXCLUDE_FROM_ALL keyword. Here's why...
# We want to be able to type simply `make` to build only the *configured* chip
# types, e.g. if model-jbay and model-runner are ON then we want `make` to
# build only libmodelcore.a, libjbay.a and the runner executables
# tofino-model[.debug]. We also need the model cmake to work as a subdirectory
# of p4factory, and so we cannot require the `make` command to be used with
# specific targets. At the p4factory level a user will want to type `make` and
# have only the configured model components built.
#
# Now, cmake includes all targets in its ALL target which is invoked by default
# when typing simply `make`. So we need to explicitly exclude some of the
# library targets from ALL using EXCLUDE_FROM_ALL and then rely on dependencies
# to cause them to be built if configured. Following the same example, the
# model-runner option causes the runner executables to be built by default. The
# model-jbay option causes the runner target to be dependent upon libjbay.a and
# so that is built by default.
#
# If the model-runner option is OFF then we want `make` to build only the
# configured chip libs (plus libmodelcore.a), but all the chip libraries have
# EXCLUDE_FROM_ALL. So, there is a 'dummy' target called configured_chips_libs
# that is included in ALL and depends upon only the configured chip libraries.
#
# It is of course possible to build any target by citing it on the `make`
# command line e.g. `make tofinoB0` will cause libtofinoB0.a to be built
# regardless of the value of the model-tofinoB0 option.

cmake_minimum_required(VERSION 3.5)
project(model LANGUAGES C CXX)


###############################################################################
# cmake build options (these do not apply to the model-autotools target)
###############################################################################
# All chip types are OFF by default, but if none are specified with cmake
# command then model-tofino will be set to ON.
# To enable a chip type pass one or more options -Dmodel-<chip> to cmake
option(model-internal "build the Model with BFN-internal extra code")
option(model-tofino "build the Model with TofinoA0 support")
option(model-tofinoB0 "build the Model with TofinoB0 support")
option(model-jbay "build the Model with JbayA0 support")
option(model-jbayB0 "build the Model with JbayB0 support")
if (NOT (${model-tofino} OR ${model-tofinoB0} OR ${model-jbay} OR ${model-jbayB0}
	))
    message("Defaulting to building model for tofinoA0 chip only")
    set(model-tofino ON)
else ()
    if (${model-tofino})
        message("Configuring model for tofinoA0 chip")
    endif ()
    if (${model-tofinoB0})
        message("Configuring model for tofinoB0 chip")
    endif ()
    if (${model-jbay})
        message("Configuring model for jbayA0 chip")
    endif ()
    if (${model-jbayB0})
        message("Configuring model for jbayB0 chip")
    endif ()
endif ()
option(precompiled-headers "precompile header files (default ON)" ON)

###############################################################################
# Common setup
###############################################################################
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_VERBOSE_MAKEFILE ON)  # uncomment to see compiler commands


# Handle top-level options.
# TODO: once integrated strip equiv section out of mimic/CMakeLists.txt
#
string (TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
if (build_type STREQUAL "release")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")   # Echo the flags that cmake will probably generate too.
  message (STATUS "MODEL Release build: -O2 (CMAKE_BUILD_TYPE=Release)")
elseif (build_type STREQUAL "debug")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
  message (STATUS "MODEL Debug build: -O0 -ggdb (CMAKE_BUILD_TYPE=Debug)")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
  message (NOTICE "MODEL unknown 'CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}'. Expected 'Release', 'Debug'")
endif()

option (model-sanitize "Turn on run-time sanitize checking" OFF)
if (${model-sanitize})
  message(STATUS "Enabling Sanitizer tooling (model-sanitize=ON)")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSANITIZER_RUNNING=1 -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer -fno-sanitize=vptr")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=pointer-compare -fsanitize=pointer-subtract")
    # TODO add: -fsanitize=memory -fsanitize-memory-track-origins
  endif ()
endif ()


find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    MESSAGE(STATUS "Enabling ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

# Optionally enable Include-What-You-Use
# See `https://wiki.ith.intel.com/display/BXDMODEL/Include+What+You+Use` for details.
if(NOT DEFINED IWYU_EXE)
    # If the user has not explicitly passed it in, see if it is defined in the environment.
    set(IWYU_EXE $ENV{IWYU_EXE})
endif()
if(EXISTS ${IWYU_EXE})
    message(STATUS "Enabling Include-What-You-Use: ${IWYU_EXE}")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXE};-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args")
    # TODO add suitable --mapping_file to reduce noise.
endif()

# add options for macOS build
if(${CMAKE_HOST_APPLE})
    set(EXTRA_CONFIGURE_OPTIONS  CXX=clang++ CC=clang)
    include_directories(AFTER SYSTEM /usr/local/include /usr/include)
endif()

if(DEFINED ENV{EC_SITE})
    include_directories(AFTER SYSTEM /p/tfc/tools/opensource/rapidjson/include)
endif()

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.54.0 REQUIRED)



message(STATUS "Using boost header-only libraries globally")
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPPFLAGS} ${CXXFLAGS} -DBOOST_LOG_DYN_LINK")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")

# default preprocessor and compiler options: each of these is applied to every
# target unless explicitly overridden for specific source files by a
# set_source_files_properties command
add_definitions(
        # Use cinttypes in rapidjson
        -DRAPIDJSON_NO_INT64DEFINE
        # Use std::string in rapidjson
        -DRAPIDJSON_HAS_STDSTRING
        )
# if -Dmodel-internal OR BFN_INTERNAL set in enviroment, compile with -DBFN_INTERNAL
if (${model-internal} OR DEFINED ENV{BFN_INTERNAL})
   add_definitions(-DBFN_INTERNAL)
endif ()

add_compile_options(
# TODO: MODEL-764: is -UNDEBUG an appropriate workaround?
# p4factory sets -DNDEBUG but model causes compiler warnings,
# e.g. from RMT_ASSERT macro, if asserts are disabled, so unset NDEBUG here
        -UNDEBUG
#
# These next two get set in CMAKE_CXX_FLAGS so not needed here???????????????????????????????????????????????????????
#        -g
#        -O2
        -Wall
        -Wno-misleading-indentation   # see MODEL-801
# use -Wextra but then switch off some of the warnings it adds...
        -Wextra
        -Wno-unused-parameter
        -Wno-ignored-qualifiers
        -Wno-type-limits
        -Wno-shadow
        -Wno-sign-compare
# Use -Werror to elevate warnings (and because p4factory does)
        -Werror
        )
# Handle problems with the old version of gcc (4.8.3) that DV uses for tofino, jbay
if(CMAKE_COMPILER_IS_GNUCC AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.4))
    # prevent gcc 4.8.3 warning about valid {} initialisers
    add_compile_options(
	-Wno-missing-field-initializers
    )
else()
    # p4factory CMakeLists.txt sets -Wpedantic so disable it here...
    # but no-pedantic triggers a spurious warning about binary constants in gcc 4.8.3
    add_compile_options(
            -Wno-pedantic
    )
endif()

# -ftemplate-depth=1024 is only valid for C++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=1024")

# set compiler specific options and create aliases for some compiler specific flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
    message("Using Clang compiler without compiler hardening flags - do not use for release")
    set(NO_UNINITIALIZED -Wno-uninitialized)
    set(NO_VAR_TRACKING )
    set(WHOLE_ARCHIVE -all_load)
    set(NO_WHOLE_ARCHIVE )
    set(FORCE_LOAD -force_load)
    set(NO_FORCE_LOAD )
    add_compile_options(
            -Wno-error
    )
else()
    set(NO_UNINITIALIZED -Wno-maybe-uninitialized)
    set(NO_VAR_TRACKING -fno-var-tracking-assignments)
    set(WHOLE_ARCHIVE --whole-archive)
    set(NO_WHOLE_ARCHIVE --no-whole-archive)
    set(FORCE_LOAD --whole-archive)
    set(NO_FORCE_LOAD --no-whole-archive)


    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fstack-protector-strong COMPILER_SUPPORTS_FSTACK_PROTECTOR_STRONG)

    if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
      if (${COMPILER_SUPPORTS_FSTACK_PROTECTOR_STRONG})
          message ("Using ${CMAKE_CXX_COMPILER_ID} compiler with compiler hardening flags")
          # MODEL-727: hardening flags required for SDLe...
          add_compile_options(-D_FORTIFY_SOURCE=2
              -fstack-protector-strong
              -Wl,-z,relro
              -Wl,-z,now)
      else()
          message ("Using ${CMAKE_CXX_COMPILER_ID} compiler which does not support compiler hardening flags - do not use for release")
      endif()
    endif()
endif()


###############################################################################
# function to add precompiled headers to a model chip library target.
# Note: cmake > v16 has built in support precompiled headers so this is here to
# support older cmake
###############################################################################
function(add_pch target hdr_file)
    # args:
    #   target: a model chip library target e.g. tofino
    #   hdr_file: absolute path to a header file that is to be precompiled
    if(NOT ${precompiled-headers})
        message("Not configuring precompiled headers")
        return()
    endif()
    get_filename_component(hdr_filename ${hdr_file} NAME)
    file(RELATIVE_PATH hdr_file_rel_path ${PROJECT_SOURCE_DIR} ${hdr_file})

    # build pch file as a library (rather than directly calling compiler
    # command) so that cmake provides all the default compiler options
    set(target_pch ${target}_pch)
    add_library(${target_pch} OBJECT EXCLUDE_FROM_ALL ${hdr_file})
    set_source_files_properties(${hdr_file}
            PROPERTIES
            LANGUAGE "CXX"
            COMPILE_FLAGS "-MMD -x c++-header"
            #        OBJECT_DEPENDS
            )
    # Note: the following assumes existence of ${target}_includes and
    # ${target}_compiler_options from which we copy the include directories and
    # compile options to use when compiling the precompiled headers.
    # Note: target_link_libraries doesn't work with OBJECT library target on
    # older cmake, so copy properties instead
    # target_link_libraries(${target_pch} PRIVATE ${target}_includes ${target}_compiler_options)
    get_target_property(tgt_includes ${target}_includes INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target_pch} PRIVATE ${tgt_includes})
    get_target_property(tgt_options ${target}_compiler_options INTERFACE_COMPILE_OPTIONS)
    target_compile_options(${target_pch} PRIVATE ${tgt_options})

    # target_pch will compile header under target_pch.dir but cmake
    # specifies the output file to have a .o extension, so we create a
    # symlink with a .gch extension pointing to the .o file
    # For example, for tofino target:
    #   include/shared/mau.h is compiled to CMakeFiles/tofino_pch.dir/include/shared/mau.h.o
    #   CMakeFiles/tofino_pch.dir/mau.h.gch is a symlink to the .o file
    set(target_pch_dir ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${target_pch}.dir)
    add_custom_target(${target}_pch_symlink
            WORKING_DIRECTORY ${target_pch_dir}
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${hdr_file_rel_path}.o ${hdr_filename}.gch
            COMMENT "symlinking ${hdr_filename}.gch -> ${hdr_file_rel_path}.o in ${target_pch_dir}"
            DEPENDS ${target_pch}
            )
    # now modify the target to search for the precompiled headers *before*
    # other include directories
    if (${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
        # clang requires precompiled headers to be specified with -include
        target_compile_options(${target} BEFORE PRIVATE -include ${target_pch_dir}/${hdr_filename})
    else ()
        target_include_directories(${target} BEFORE PRIVATE ${target_pch_dir})
    endif ()
    # if compiler does not find ${hdr_filename}.gch then it will look for
    # ${hdr_filename}.h *in precompiled header location*, so we can detect missing
    # ${hdr_filename}.gch file by creating a bad ${hdr_filename} in the same
    # directory where ${hdr_filename}.gch file should be
    # MODEL-753: CLion indexing doesn't like the #error directive so soften to #warning
    # file(WRITE ${target_pch_dir}/${hdr_filename} "\#error \"Precompiled header for ${hdr_filename} not found\"")
    file(WRITE ${target_pch_dir}/${hdr_filename} "\#warning \"Precompiled header for ${hdr_filename} not found\"")
    # finally, add the targets created by this function as dependencies of
    # ${target}
    add_dependencies(${target} ${target}_pch_symlink)
endfunction()


###############################################################################
# target to generate the model-version.h file
###############################################################################
# Note: the file is only modified if its current content would change
set(MODEL_VERSION_HEADER_FILE ${PROJECT_SOURCE_DIR}/include/model_core/model-version.h)
add_custom_target(generate_version_file
        COMMENT "Generating ${MODEL_VERSION_HEADER_FILE}"
        COMMAND ./tools/build_generated_versions.sh . MODEL --match model --output ${MODEL_VERSION_HEADER_FILE}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )


###############################################################################
# build useful re-usable lists/targets based on configured chip types
###############################################################################
# Note: configured_chips is a list rather than a library target because using
# the latter caused the configured_chips to be added to link libs list *last*
# regardless of when cited e.g. in the runner
list(APPEND configured_chips)
add_library(configured_chips_compiler_options INTERFACE)
if (${model-tofino})
    list(APPEND configured_chips tofino)
    target_compile_options(configured_chips_compiler_options INTERFACE -DMODEL_TOFINO)
endif ()
if (${model-tofinoB0})
    list(APPEND configured_chips tofinoB0)
    target_compile_options(configured_chips_compiler_options INTERFACE -DMODEL_TOFINOB0)
endif ()
if (${model-jbay})
    list(APPEND configured_chips jbay)
    target_compile_options(configured_chips_compiler_options INTERFACE -DMODEL_JBAY)
endif ()
if (${model-jbayB0})
    list(APPEND configured_chips jbayB0)
    target_compile_options(configured_chips_compiler_options INTERFACE -DMODEL_JBAYB0)
endif ()


###############################################################################
# target libmodelcore
###############################################################################
add_library(modelcore STATIC)
add_dependencies(modelcore generate_version_file)
target_include_directories (modelcore PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        )
target_sources(modelcore PRIVATE
        ${PROJECT_SOURCE_DIR}/src/model_core/capture.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/chip_interface.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/event.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/file-logger.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/json-loader.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/log-buffer.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/model.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/p4-objects.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/port-json-loader.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/register_block.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/rmt-debug.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/rmt-global.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/rmt-phv-modification.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/rmt-types.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/rmt-util.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/rmt.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/spinlock.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/timer-manager.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/timer.cpp
        ${PROJECT_SOURCE_DIR}/src/model_core/version-string.cpp)
target_link_libraries(modelcore PRIVATE configured_chips_compiler_options)

###############################################################################
# Source file listings for per-chip library targets
# Note: these are maintained as vars so that they can be re-used for different
# chip targets
###############################################################################

#### shared ####
set(SHARED_SOURCES
        ${PROJECT_SOURCE_DIR}/src/shared/action-output-hv-xbar.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/afc-metadata-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/bitvector.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/checksum-engine-shared.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/chip.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/epb-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/hash-address-vh-xbar-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/indirect_access_block.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/instr.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/ipb-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/learning-filter.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/learning-filter-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mac-counters.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/match-data-input-vh-xbar-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-addr-dist.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-color-switchbox.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-table-counters.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-defs.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-delay.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-dependencies.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-execute-step.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-gateway-payload.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-gateway-payload-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-gateway-table.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-gateway-table-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-hash-distribution.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-hash-generator-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-info.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-input-xbar-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-instr-store-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-row.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-row-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-table.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-table-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-tcam.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-tcam-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-lookup-result.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-lpf-meter.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-mapram.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-mapram-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-memory.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-meter.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-meter-alu.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-moveregs.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-object.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-op-handler-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-predication-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-result-bus.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sbox.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-selector-alu.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-snapshot-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sram-column.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sram-column-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sram.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sram-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sram-row.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-sram-row-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-stash.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-stash-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-stash-column.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-stash-column-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-stateful-alu.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-stats-alu.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-tcam.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-tcam-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-tcam-row.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-teop-common.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mirror.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/p4-name-lookup.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/packet.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/packet-body.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/packet-buffer.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/packet-pipe-data.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/packet-replication-engine.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/packet-replication-engine-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/parser-block.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/parser.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/parser-static-config.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/phv.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/phv-modification.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/phv-pipe-data.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/phv-factory.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/pipe.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/pipe-object.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/port.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/queueing.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rdm.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-config.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-defs.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-log.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-object.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-object-manager.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-string-map.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-op-handler.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-packet-coordinator.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-sweeper.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/tcam-row-vh-xbar-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/tofino_lfsr.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/event.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/rmt-event-emitter.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-event-emitter.cpp
        )

#### tofinoXX and jbay ####
set(TOFINOXX_JBAY_SOURCES
        ${PROJECT_SOURCE_DIR}/src/tofinoXX_jbay/comira-umac3.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX_jbay/comira-umac4.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX_jbay/mau-logical-tcam-col.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX_jbay/mau-mpr-tcam-table.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX_jbay/mirror.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX_jbay/queueing.cpp
        )

#### tofinoxx only ####
set(TOFINOXX_SOURCES
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/checksum-engine.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/chip.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-block.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-chip-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-egress.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-ingress.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-learning.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/epb.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/instr-opcode.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/ipb.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mac-factory.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-chip-lpf-meter.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-chip-stateful-alu.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-chip-tcam-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-gateway-payload-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-hash-generator-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-ignored-regs.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-instr-store.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-op-handler.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-predication.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-snapshot.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-stash.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-stateful-counters.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-teop.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/parser-arbiter.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/parser.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/pktgen.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/pktgen-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/pktgen-app-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/rdm-chip.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/tcam-row-vh-xbar-with-reg.cpp
        )

#### jbay shared ####
set(JBAY_SHARED_SOURCES
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/checksum-engine.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/chip.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-block.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-egress.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-ingress.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-learning.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/epb.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/instr-opcode.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-chip-addr-dist.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-chip-lpf-meter.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-chip-result-bus.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-chip-stateful-alu.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-gateway-payload-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-hash-generator-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-instr-store.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-mapram.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-op-handler.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-predication.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-snapshot.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-sram-row-chip-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-stash.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-stateful-counters.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mau-teop.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/mirror-buffer-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/p2s.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/parse-merge-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/parser-arbiter.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/parser.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/pktgen-app-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/pktgen-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/pktgen.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/rate-lim.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tcam-row-vh-xbar-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-object.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-pex.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-qlc.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-config.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-exc-rate.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-max-rate.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-min-rate.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-model-wrapper.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-node-arb.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-occ.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-pex-cr.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-port-arb.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-sel.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch-state.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/tm-sch.cpp
        )

#### jbayXX sources ####
set(JBAYXX_SOURCES
        ${PROJECT_SOURCE_DIR}/src/jbay/deparser-chip-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/deparser-ingress.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/egress-buf.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/epb-counters.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/ipb.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/mac-factory.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/mau-chip-tcam-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbayXX/mau-ignored-regs.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/parse-merge-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/parser.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/s2p.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay/rdm-chip.cpp
        )



# special case some source files to ignore some compiler warnings
# e.g. ignore unitialized var warnings where register adapter pattern is used
set_source_files_properties(
        ${PROJECT_SOURCE_DIR}/src/shared/mau-logical-table-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/mau-gateway-payload-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/shared/tcam-row-vh-xbar-with-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/deparser-reg.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/ipb.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-snapshot.cpp
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-reg.cpp
        PROPERTIES COMPILE_FLAGS ${NO_UNINITIALIZED}
)
set_source_files_properties(
        ${PROJECT_SOURCE_DIR}/src/jbay_shared/deparser-reg.cpp
        PROPERTIES COMPILE_FLAGS ${NO_UNINITIALIZED}
)
set_source_files_properties(
        ${PROJECT_SOURCE_DIR}/src/tofinoXX/mau-ignored-regs.cpp
        ${PROJECT_SOURCE_DIR}/src/jbayXX/mau-ignored-regs.cpp
        PROPERTIES COMPILE_FLAGS "${NO_UNINITIALIZED} ${NO_VAR_TRACKING}"
)

###############################################################################
# target libtofino
###############################################################################
add_library(tofino_compiler_options INTERFACE)
target_compile_options(tofino_compiler_options INTERFACE
        -DMODEL_TOFINO
        -DMODEL_CHIP_NAMESPACE=tofino
        )

add_library(tofino_includes INTERFACE)
target_include_directories(tofino_includes INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/tofino
        ${PROJECT_SOURCE_DIR}/include/tofino/dummy_registers
        ${PROJECT_SOURCE_DIR}/include/tofinoXX
        ${PROJECT_SOURCE_DIR}/include/tofinoXX_jbay
        ${PROJECT_SOURCE_DIR}/include/shared
        ${PROJECT_SOURCE_DIR}/include/tofino/register_includes
        )

add_library(tofino STATIC EXCLUDE_FROM_ALL)
add_dependencies(tofino generate_version_file)
add_pch(tofino ${PROJECT_SOURCE_DIR}/include/shared/mau.h)
target_link_libraries(tofino PRIVATE tofino_includes tofino_compiler_options)
target_sources(tofino PRIVATE
        ${SHARED_SOURCES}
        ${TOFINOXX_JBAY_SOURCES}
        ${TOFINOXX_SOURCES}
        ${PROJECT_SOURCE_DIR}/src/tofino/deparser-ingress.cpp
        ${PROJECT_SOURCE_DIR}/src/tofino/mau-mapram.cpp
        ${PROJECT_SOURCE_DIR}/src/tofino/mirror-buffer-reg.cpp
        )

###############################################################################
# target libtofinoB0
###############################################################################
add_library(tofinoB0_compiler_options INTERFACE)
target_compile_options(tofinoB0_compiler_options INTERFACE
        -DMODEL_TOFINOB0
        -DMODEL_CHIP_NAMESPACE=tofinoB0
        )

add_library(tofinoB0_includes INTERFACE)
target_include_directories(tofinoB0_includes INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/tofinoB0
        ${PROJECT_SOURCE_DIR}/include/tofinoB0/dummy_registers
        ${PROJECT_SOURCE_DIR}/include/tofinoXX
        ${PROJECT_SOURCE_DIR}/include/tofinoXX_jbay
        ${PROJECT_SOURCE_DIR}/include/shared
        ${PROJECT_SOURCE_DIR}/include/tofinoB0/register_includes
        )

add_library(tofinoB0 STATIC EXCLUDE_FROM_ALL)
add_pch(tofinoB0 ${PROJECT_SOURCE_DIR}/include/shared/mau.h)
add_dependencies(tofinoB0 generate_version_file)
target_link_libraries(tofinoB0 PRIVATE tofinoB0_includes tofinoB0_compiler_options)
target_sources(tofinoB0 PRIVATE
        ${SHARED_SOURCES}
        ${TOFINOXX_JBAY_SOURCES}
        ${TOFINOXX_SOURCES}
        ${PROJECT_SOURCE_DIR}/src/tofinoB0/deparser-ingress.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoB0/mau-mapram.cpp
        ${PROJECT_SOURCE_DIR}/src/tofinoB0/mirror-buffer-reg.cpp
        )

###############################################################################
# target libjbay
###############################################################################
add_library(jbay_compiler_options INTERFACE)
target_compile_options(jbay_compiler_options INTERFACE
        -DMODEL_JBAY
        -DMODEL_CHIP_NAMESPACE=jbay
        )

add_library(jbay_includes INTERFACE)
target_include_directories(jbay_includes INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/jbay
        ${PROJECT_SOURCE_DIR}/include/jbay/dummy_registers
        ${PROJECT_SOURCE_DIR}/include/jbayXX
        ${PROJECT_SOURCE_DIR}/include/jbay_shared
        ${PROJECT_SOURCE_DIR}/include/tofinoXX_jbay
        ${PROJECT_SOURCE_DIR}/include/shared
        ${PROJECT_SOURCE_DIR}/include/jbay/register_includes
        )

add_library(jbay STATIC EXCLUDE_FROM_ALL)
add_pch(jbay ${PROJECT_SOURCE_DIR}/include/shared/mau.h)
add_dependencies(jbay generate_version_file)
target_link_libraries(jbay PRIVATE jbay_includes jbay_compiler_options)
target_sources(jbay PRIVATE
        ${SHARED_SOURCES}
        ${TOFINOXX_JBAY_SOURCES}
        ${JBAY_SHARED_SOURCES}
        ${JBAYXX_SOURCES}
        )

###############################################################################
# target libjbayB0
###############################################################################
add_library(jbayB0_compiler_options INTERFACE)
target_compile_options(jbayB0_compiler_options INTERFACE
        -DMODEL_JBAYB0
        -DMODEL_CHIP_NAMESPACE=jbayB0
        )

add_library(jbayB0_includes INTERFACE)
target_include_directories(jbayB0_includes INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/jbayB0
        ${PROJECT_SOURCE_DIR}/include/jbayB0/dummy_registers
        ${PROJECT_SOURCE_DIR}/include/jbayXX
        ${PROJECT_SOURCE_DIR}/include/jbay_shared
        ${PROJECT_SOURCE_DIR}/include/tofinoXX_jbay
        ${PROJECT_SOURCE_DIR}/include/shared
        ${PROJECT_SOURCE_DIR}/include/jbayB0/register_includes
        )

add_library(jbayB0 STATIC EXCLUDE_FROM_ALL)
add_pch(jbayB0 ${PROJECT_SOURCE_DIR}/include/shared/mau.h)
add_dependencies(jbayB0 generate_version_file)
target_link_libraries(jbayB0 PRIVATE jbayB0_includes jbayB0_compiler_options)
target_sources(jbayB0 PRIVATE
        ${SHARED_SOURCES}
        ${TOFINOXX_JBAY_SOURCES}
        ${JBAY_SHARED_SOURCES}
        ${JBAYXX_SOURCES}
        )


###############################################################################
# target to build all the *configured* chip libs; this target is included in
# the default targets (`all`) and so will be built by `make`
#
# Note: libs for chips that are not configured can be built by citing their
# targets on the make command line e.g. `make jbayB0`
###############################################################################
add_custom_target(configured_chips_libs ALL DEPENDS ${configured_chips})


###############################################################################


# pull in runner
# Note: do this *before* tests/utests because ${configure_model_runner} is set in
# runner and used in tests/utests
add_subdirectory(runner)
# pull in utests
add_subdirectory(tests/wrapper)
add_subdirectory(tests/utests)
add_subdirectory(tests/simple_test_harness)




###############################################################################
# External project to build model using legacy autotools build chain
###############################################################################
# The model-autotools target enables the legacy autotools build process
# to be executed from cmake. It is now excluded from the default targets
# so will need to be explicitly built using:
#
#   make model-autotools
#
# Set the option that will be passed to the model autotools configure command;
# the defaults can be overridden by using cmake options -DCONFIGURE_OPTIONS,
# for example:
#  cmake -DCONFIGURE_OPTIONS="--enable-model-jbay --disable-simple-test-harness --disable-model-dv-utests"
set(CONFIGURE_OPTIONS
        "--enable-model-tofinoB0 --disable-model-dv-utests"
        CACHE STRING
        "Options to pass to autotools configure (applies to model-autotools target only)")
separate_arguments(CONFIGURE_OPTIONS)

# build the model in a sub-directory of the cmake build dir to avoid clashing
# Makefiles; typically this will something like <repo dir>/cmake-build/build
set(MODEL_BUILD_DIR ${CMAKE_BINARY_DIR}/build)

# use an external project to drive the model's autotools autogen, configure and
# make steps
include(ExternalProject)
# Note: specifying '-j4' in the BUILD_COMMAND below provokes "warning: -jN
# forced in submake: disabling jobserver mode." because the model 'make'
# command is being called from the cmake 'make' command. However, the model
# 'make' does appear to execute 4 parallel jobs.
# Note: specifying 'TESTS=' in the BUILD_COMMAND prevents any tests being
# automatically run by 'make' after the test target has been built
ExternalProject_Add(model-autotools
        SOURCE_DIR ${PROJECT_SOURCE_DIR}
        BINARY_DIR ${MODEL_BUILD_DIR}
        CONFIGURE_COMMAND  ${PROJECT_SOURCE_DIR}/configure ${CONFIGURE_OPTIONS} ${EXTRA_CONFIGURE_OPTIONS}
        BUILD_COMMAND make -j4
        BUILD_ALWAYS true
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL true
        )
ExternalProject_Add_Step(model-autotools configure-message
        COMMENT "Displaying configure options"
        DEPENDEES autogen
        DEPENDERS configure
        COMMAND echo "Using autotools configure options: ${CONFIGURE_OPTIONS} ${EXTRA_CONFIGURE_OPTIONS}"
        )
ExternalProject_Add_Step(model-autotools autogen
        COMMENT "Performing autogen step for model-autotools"
        DEPENDEES patch
        DEPENDERS configure-message
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ./autogen.sh
        )
ExternalProject_Add_Step(model-autotools make-testcheck
        COMMENT "Performing test compilation step for model-autotools"
        DEPENDEES build
        DEPENDERS install
        WORKING_DIRECTORY ${MODEL_BUILD_DIR}
        COMMAND make test_reader_O3 && make -j4 testcheck TESTS=
        )
ExternalProject_Add(model-autotools-clean
        SOURCE_DIR ${PROJECT_SOURCE_DIR}
        BINARY_DIR ${MODEL_BUILD_DIR}
        CONFIGURE_COMMAND  ""
        BUILD_COMMAND make clean
        BUILD_ALWAYS true
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL true
        )

###############################################################################
