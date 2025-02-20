# First, check whether the repository needs to be initialized.
# Check if the submodule directory exists.
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/p4c/CMakeLists.txt")
    message(STATUS "Initializing P4C submodule...")
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMODULE_RESULT
    )

    if (NOT GIT_SUBMODULE_RESULT EQUAL "0")
        message(FATAL_ERROR "Failed to initialize P4C submodule. Exiting.")
    endif()
else()
    message(STATUS "P4C submodule already initialized.")
endif()


add_definitions("-DBAREFOOT_INTERNAL=1")
add_definitions("-DHAVE_JBAY=1")
set (P4C_USE_PREINSTALLED_ABSEIL ON CACHE BOOL "")
set (P4C_USE_PREINSTALLED_PROTOBUF ON CACHE BOOL "")
set (ENABLE_PROTOBUF_STATIC ON CACHE BOOL "")


set (CMAKE_CXX_FLAGS "")
set (HAVE_LIBBOOST_GRAPH 1)
add_definitions("-DHAVE_LIBBOOST_GRAPH=1")
set (ENABLE_TOFINO ON CACHE BOOL "")
set (ENABLE_GTESTS OFF CACHE BOOL "")
set (ENABLE_BMV2 OFF CACHE BOOL "")
set (ENABLE_EBPF OFF CACHE BOOL "")
set (ENABLE_UBPF OFF CACHE BOOL "")
set (ENABLE_DPDK OFF CACHE BOOL "")
set (ENABLE_P4TC OFF CACHE BOOL "")
set (ENABLE_P4FMT OFF CACHE BOOL "")
set (ENABLE_P4TEST OFF CACHE BOOL "")
set (ENABLE_P4C_GRAPHS OFF CACHE BOOL "")
# set (ENABLE_LTO ON CACHE BOOL "")
# Enable Unity builds for the compiler to speed up compilation.
set(CMAKE_UNITY_BUILD_PREV ${CMAKE_UNITY_BUILD})
set(CMAKE_UNITY_BUILD ON)
add_subdirectory(p4c)
set(CMAKE_UNITY_BUILD ${CMAKE_UNITY_BUILD_PREV})


# FIXME: Temporary hack to link p4c-barefoot into the appropriate installation directory.
# There must be a better way to do this reliably. Ideally, the standalone build should be using
# the p4c-barefoot binary that is created in the build directory instead of the globally installed bf-p4c.
add_custom_target(link-bf-p4c
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/p4c/p4c ${P4C}
  DEPENDS p4c-barefoot p4c_driver
)
add_dependencies(link-bf-p4c p4c-barefoot p4c_driver)
add_dependencies(bf-p4c link-bf-p4c update_includes)
