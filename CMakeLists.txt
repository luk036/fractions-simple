cmake_minimum_required(VERSION 3.14)
project(fractions-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(FRACTION_BUILD_TESTS    "Build tests"              ON)
option(FRACTION_ENABLE_COVERAGE "Enable coverage reporting" OFF)

# ---------------------------------------------------------------------------
# Header-only library target
# ---------------------------------------------------------------------------
add_library(fraction INTERFACE)
target_include_directories(fraction INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
enable_testing()

if(FRACTION_BUILD_TESTS)
    # Download doctest header directly (avoids doctest's own CMake issues)
    set(DOCTEST_VERSION 2.4.11)
    set(DOCTEST_DIR "${CMAKE_BINARY_DIR}/_deps/doctest")
    set(DOCTEST_HEADER "${DOCTEST_DIR}/doctest/doctest.h")

    if(NOT EXISTS "${DOCTEST_HEADER}")
        file(MAKE_DIRECTORY "${DOCTEST_DIR}/doctest")
        file(
            DOWNLOAD
            "https://raw.githubusercontent.com/doctest/doctest/v${DOCTEST_VERSION}/doctest/doctest.h"
            "${DOCTEST_HEADER}"
            STATUS download_status
        )
        list(GET download_status 0 status_code)
        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR "Failed to download doctest (status ${status_code})")
        endif()
    endif()

    add_library(doctest INTERFACE)
    target_include_directories(doctest INTERFACE "${DOCTEST_DIR}")

    add_executable(test_fraction tests/test_fraction.cpp)
    target_link_libraries(test_fraction PRIVATE fraction doctest)

    if(MSVC)
        target_compile_options(test_fraction PRIVATE /utf-8)
    endif()

    if(FRACTION_ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
            target_compile_options(test_fraction PRIVATE --coverage -fno-inline -fno-elide-constructors)
            target_link_options(test_fraction PRIVATE --coverage)
        endif()
    endif()

    add_test(NAME test_fraction COMMAND test_fraction)
endif()

# ---------------------------------------------------------------------------
# Coverage report target (GCC/Clang only — requires gcovr)
# ---------------------------------------------------------------------------
if(FRACTION_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    find_program(GCOVR_PATH gcovr)
    if(GCOVR_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${GCOVR_PATH} --root ${PROJECT_SOURCE_DIR}
                --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
                --exclude tests/
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report in ${CMAKE_BINARY_DIR}/coverage/index.html"
        )
    else()
        message(WARNING "gcovr not found — coverage target unavailable. Install with: pip install gcovr")
    endif()
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
