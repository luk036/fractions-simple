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
CMakeLists.txt            # CMake build (FetchContent for doctest)
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

## Before Committing

1. Run `xmake f -m release && xmake run test_fraction` (or CMake equivalent)
2. Ensure all 45 test cases and 190+ assertions pass
