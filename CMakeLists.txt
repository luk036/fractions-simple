cmake_minimum_required(VERSION 3.14)
project(
  fractions-simple
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Compiler warnings (as errors)
# ---------------------------------------------------------------------------

function(target_enable_warnings TARGET)
  if(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4819)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
endfunction()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(FRACTION_BUILD_TESTS "Build tests" ON)
option(FRACTION_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(FRACTION_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(FRACTION_ENABLE_FORMAT "Enable format targets" ON)

# ---------------------------------------------------------------------------
# Header-only library target
# ---------------------------------------------------------------------------
add_library(fraction INTERFACE)
target_include_directories(
  fraction INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
    file(DOWNLOAD
         "https://raw.githubusercontent.com/doctest/doctest/v${DOCTEST_VERSION}/doctest/doctest.h"
         "${DOCTEST_HEADER}" STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
      message(FATAL_ERROR "Failed to download doctest (status ${status_code})")
    endif()
  endif()

  add_library(doctest INTERFACE)
  target_include_directories(doctest SYSTEM INTERFACE "${DOCTEST_DIR}")
  if(MSVC)
    target_compile_options(doctest INTERFACE /wd5285)
  endif()

  add_executable(test_fraction tests/test_fraction.cpp)
  target_link_libraries(test_fraction PRIVATE fraction doctest)
  target_enable_warnings(test_fraction)

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
# Benchmarks
# ---------------------------------------------------------------------------

if(FRACTION_BUILD_BENCHMARKS)
  set(NANOBENCH_VERSION 4.3.11)
  set(NANOBENCH_DIR "${CMAKE_BINARY_DIR}/_deps/nanobench")
  set(NANOBENCH_HEADER "${NANOBENCH_DIR}/nanobench.h")

  if(NOT EXISTS "${NANOBENCH_HEADER}")
    file(MAKE_DIRECTORY "${NANOBENCH_DIR}")
    file(DOWNLOAD
         "https://raw.githubusercontent.com/martinus/nanobench/v${NANOBENCH_VERSION}/src/include/nanobench.h"
         "${NANOBENCH_HEADER}" STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
      message(FATAL_ERROR "Failed to download nanobench (status ${status_code})")
    endif()
  endif()

  add_library(nanobench INTERFACE)
  target_include_directories(nanobench SYSTEM INTERFACE "${NANOBENCH_DIR}")

  add_executable(benchmark_fraction tests/benchmark_fraction.cpp)
  target_link_libraries(benchmark_fraction PRIVATE fraction nanobench)
  target_enable_warnings(benchmark_fraction)

  add_custom_target(
    run_benchmark
    COMMAND benchmark_fraction
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running benchmark_fraction"
  )
endif()

# ---------------------------------------------------------------------------
# Coverage report target (GCC/Clang only — requires gcovr)
# ---------------------------------------------------------------------------
if(FRACTION_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  find_program(GCOVR_PATH gcovr)
  if(GCOVR_PATH)
    set(extra_gcovr_args "")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
      find_program(LLVM_COV_PATH llvm-cov)
      if(LLVM_COV_PATH)
        list(APPEND extra_gcovr_args --llvm-cov-binary ${LLVM_COV_PATH})
      endif()
    endif()
    add_custom_target(
      coverage
      COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
      COMMAND ${GCOVR_PATH} --root ${PROJECT_SOURCE_DIR} --html-details
              ${CMAKE_BINARY_DIR}/coverage/index.html --exclude tests/ ${extra_gcovr_args}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating coverage report in ${CMAKE_BINARY_DIR}/coverage/index.html"
    )
  else()
    message(
      WARNING "gcovr not found — coverage target unavailable. Install with: pip install gcovr"
    )
  endif()
endif()

# ---------------------------------------------------------------------------
# Format targets (clang-format + cmake-format)
# ---------------------------------------------------------------------------

if(FRACTION_ENABLE_FORMAT)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)
  CPMAddPackage("gh:TheLartians/Format.cmake@1.7.3")
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN "*.hpp"
)
