# Agent Guidelines for fractions-simple

## Project Overview

C++ header-only library for fractional number arithmetic with generic signed integer types.
Port of [fractions-rs](https://github.com/luk036/fractions-rs).

## Build & Test Commands

### Primary — xmake

```bash
xmake f -m release    # configure
xmake                 # build
xmake run test_fraction  # run all tests
```

### Secondary — CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## Project Structure

```
include/
└── fraction/
    └── fraction.hpp      # Single-header Fraction<T> (C++17, header-only)
tests/
└── test_fraction.cpp     # 45 doctest test cases (~190 assertions)
CMakeLists.txt            # CMake build (FetchContent for doctest, Format.cmake for formatting)
cmake/
└── CPM.cmake             # CPM dependency manager
xmake.lua                 # xmake build (preferred)
```

## Code Style Guidelines

### Naming Conventions

- **Functions/Methods**: snake_case (e.g., `normalize`, `keep_denom_positive`)
- **Types**: PascalCase (e.g., `Fraction`, template parameter `T`)
- **Predicates**: `is_*` prefix (e.g., `is_zero()`, `is_infinite()`, `is_nan()`)
- **Mutators**: `set_*` prefix (e.g., `set_zero()`, `set_infinite()`)
- **Tests**: Use descriptive `TEST_CASE("name")` strings with doctest

### Code Formatting

- 4-space indentation
- K&R brace style (opening brace on same line)
- Spaces around operators and after commas
- No trailing whitespace
- Formatting enforced via `.clang-format` (Google-derived) and `.cmake-format`

### Includes

- Standard library headers first (`<cstdlib>`, `<type_traits>`, etc.)
- Library headers (`<fraction/fraction.hpp>`)
- Test framework (`<doctest/doctest.h>`) — test files only

### Template Constraints

- `Fraction<T>` requires `std::is_integral_v<T> && std::is_signed_v<T>`
- Use `static_assert` for constraint enforcement
- Use `std::enable_if` for SFINAE on free function templates

## Fraction Semantics

### Special Values

| Value | numer | denom |
|-------|-------|-------|
| Zero | 0 | ≠0 |
| One | n | n (n≠0) |
| Infinity | ±1 | 0 |
| NaN | 0 | 0 |

### Invariants

- Denominator is always kept positive (sign stored in numerator)
- Fractions are always reduced to lowest terms (after `from()` or `normalize()`)
- Raw construction (`from_raw()`) bypasses normalization

### Testing

- All 45 test cases use doctest with `CHECK` (non-fatal) assertions
- Tests cover: construction, normalization, arithmetic, comparison,
  special values (NaN/Inf/zero), cross product, stream output, archimedes
- Tested integer types: `int`, `int8_t`, `int64_t`

## Coverage

### xmake

```bash
xmake f -m coverage       # configure in coverage mode
xmake                     # build with instrumentation
xmake run test_fraction   # run tests (generates .gcda files)
xmake coverage            # generate HTML report
```

### CMake (requires GCC/Clang + gcovr)

```bash
cmake -B build -DFRACTION_ENABLE_COVERAGE=ON -DFRACTION_BUILD_TESTS=ON
cmake --build build
cmake --build build --target coverage   # runs tests + generates report
```

Report is written to `build/coverage/index.html`.

## Formatting

### CMake (uses Format.cmake via CPM)

```bash
cmake -B build -DFRACTION_BUILD_TESTS=OFF
cmake --build build --target format      # check formatting (dry-run)
cmake --build build --target fix-format  # apply formatting fixes
cmake --build build --target check-format # exit 1 if unformatted (CI use)
```

### xmake

```bash
xmake format          # check formatting (dry-run)
xmake fix-format      # apply formatting fixes
```

### Requirements

```bash
pip install clang-format cmake_format pyyaml
```

## Benchmarks

### xmake

```bash
xmake f -m release    # configure in release mode
xmake                 # build
xmake run benchmark_fraction  # run benchmarks
```

### CMake

```bash
cmake -B build -DFRACTION_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --build build --target run_benchmark   # run benchmarks
# or directly:
./build/benchmark_fraction
```

Uses [nanobench](https://github.com/martinus/nanobench) v4.3.11 (single-header microbenchmark library).
Benchmarks cover: const_gcd, construction, arithmetic, comparison, cross product, archimedes, and normalization — across `int` and `int64_t` types.

## Doxygen Documentation

### xmake

```bash
xmake doxygen    # generate HTML docs in docs/doxygen/
```

### CMake

```bash
cmake -B build -DFRACTION_BUILD_DOCS=ON
cmake --build build --target doxygen  # generate HTML docs in docs/doxygen/
```

Requires [Doxygen](https://www.doxygen.nl/) (`pip install doxygen`).
Generated HTML is at `docs/doxygen/html/index.html`.

## clang-tidy

### xmake

```bash
xmake clang-tidy    # run clang-tidy static analysis
```

### CMake

```bash
cmake -B build -DFRACTION_ENABLE_CLANG_TIDY=ON
cmake --build build --target clang-tidy
```

Configuration in `.clang-tidy` enables clang-diagnostic, clang-analyzer, bugprone, modernize,
performance, and readability check groups — all warnings treated as errors.

## Before Committing

1. Run `xmake f -m release && xmake run test_fraction` (or CMake equivalent)
2. Run `xmake format` to check formatting
3. Ensure all 45 test cases and 190+ assertions pass
4. If benchmarks were modified, verify with `xmake run benchmark_fraction`
