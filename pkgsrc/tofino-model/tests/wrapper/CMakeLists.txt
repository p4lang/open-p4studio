option(model-wrapper "enable model wrappers" ON)
if (NOT ${model-wrapper})
    message("Not configuring model wrapper targets")
    return()
else ()
    message("Configuring model wrapper targets")
endif ()

add_compile_options(
        # disable -Werror in this directory
        -Wno-error
)
set(WRAPPER_COMMON_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/dprsr_meta_data_enum.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ref_model_dpi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ref_model_dpi.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ref_model_wrapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ref_model_wrapper.hpp
        ${PROJECT_SOURCE_DIR}/include/wrapper/sknobs.h
        )

#### tofino ####
add_library(tofinowrapper STATIC EXCLUDE_FROM_ALL)
target_sources(tofinowrapper PRIVATE
        ${WRAPPER_COMMON_SOURCES}
        )
target_link_libraries(tofinowrapper PRIVATE tofino_includes tofino_compiler_options)
target_include_directories(tofinowrapper PRIVATE
        ${PROJECT_SOURCE_DIR}/tests
)
target_compile_options(tofinowrapper PRIVATE
        -DINCLUDED_FROM_TEST_WRAPPER
        )

#### tofinoB0 ####
add_library(tofinoB0wrapper STATIC EXCLUDE_FROM_ALL)
target_sources(tofinoB0wrapper PRIVATE
        ${WRAPPER_COMMON_SOURCES}
        )
target_link_libraries(tofinoB0wrapper PRIVATE tofinoB0_includes tofinoB0_compiler_options)
target_include_directories(tofinoB0wrapper PRIVATE
        ${PROJECT_SOURCE_DIR}/tests
)
target_compile_options(tofinoB0wrapper PRIVATE
        -DINCLUDED_FROM_TEST_WRAPPER
        )

#### jbay ####
add_library(jbaywrapper STATIC EXCLUDE_FROM_ALL)
target_sources(jbaywrapper PRIVATE
        ${WRAPPER_COMMON_SOURCES}
        )
target_link_libraries(jbaywrapper PRIVATE jbay_includes jbay_compiler_options)
target_include_directories(jbaywrapper PRIVATE
        ${PROJECT_SOURCE_DIR}/tests
)
target_compile_options(jbaywrapper PRIVATE
        -DINCLUDED_FROM_TEST_WRAPPER
        )

#### jbayB0 ####
add_library(jbayB0wrapper STATIC EXCLUDE_FROM_ALL)
target_sources(jbayB0wrapper PRIVATE
        ${WRAPPER_COMMON_SOURCES}
        )
target_link_libraries(jbayB0wrapper PRIVATE jbayB0_includes jbayB0_compiler_options)
target_include_directories(jbayB0wrapper PRIVATE
        ${PROJECT_SOURCE_DIR}/tests
)
target_compile_options(jbayB0wrapper PRIVATE
        -DINCLUDED_FROM_TEST_WRAPPER
        )


###############################################################################
# target to build wrapper lib for all the *configured* chip libs; this target
# is included in the default targets (`all`) and so will be built by `make`
#
# Note: wrapper libs for chips that are not configured can be built by citing
# their targets on the make command line e.g. `make jbayB0wrapper`
# add_custom_target(configured_wrappers ALL)
###############################################################################
add_custom_target(configured_wrapper_libs ALL)
foreach (chip ${configured_chips})
    add_dependencies(configured_wrapper_libs ${chip}wrapper)
endforeach ()

