# fractions-cpp

C++ port of [fractions-rs](https://github.com/luk036/fractions-rs) — a header-only library for
fractional number arithmetic with generic signed integer types.

## Features

- Header-only — no compiled library required
- Generic `Fraction<T>` template for any signed integer type (`int`, `int64_t`, `int8_t`, etc.)
- Full set of arithmetic operators (`+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`)
- Normalized representation (denominator always positive, reduced to lowest terms)
- Special value support: zero, one, infinity, NaN
- Cross product, absolute value, signum, reciprocal
- `constexpr`-friendly — usable at compile time
- `operator<<` for stream output (e.g. `"3/4"`, `"inf"`, `"nan"`)

## Requirements

- C++17 or later
- [doctest](https://github.com/doctest/doctest) (automatically fetched for tests)

## Quick Start

```cpp
#include <fraction/fraction.hpp>
#include <iostream>

using fraction::Fraction;

int main() {
    auto a = Fraction<int>::from(1, 2);   // 1/2
    auto b = Fraction<int>::from(1, 3);   // 1/3

    auto c = a + b;                        // 5/6
    std::cout << a << " + " << b << " = " << c << "\n";

    auto d = a * b;                        // 1/6
    std::cout << a << " * " << b << " = " << d << "\n";

    auto e = Fraction<int>::from_raw(1, 0); // infinity
    std::cout << "infinity: " << e << "\n";

    auto f = Fraction<int>::from_raw(0, 0); // NaN
    std::cout << "NaN: " << f << "\n";
}
```

## Build & Test

### xmake (primary)

```bash
xmake f -m release
xmake
xmake run test_fraction
```

### CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

## Project Structure

```
include/
└── fraction/
    └── fraction.hpp      # Single-header implementation
tests/
└── test_fraction.cpp     # 45 doctest test cases
CMakeLists.txt            # CMake build (FetchContent for doctest)
xmake.lua                 # xmake build
```

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.
