add_compile_options(
        # disable -Werror in this directory
        -Wno-error
)

### modelrunner lib tests
add_library(modelrunnertest STATIC EXCLUDE_FROM_ALL)
target_sources(modelrunnertest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_runner.cpp
        )
target_include_directories(modelrunnertest PRIVATE
        ${PROJECT_SOURCE_DIR}/runner
        ${PROJECT_SOURCE_DIR}/tests/utests  # for gtest.h
        )
