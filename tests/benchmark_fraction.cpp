#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <cstdint>
#include <fraction/fraction.hpp>

using fraction::archimedes;
using fraction::const_abs;
using fraction::const_gcd;
using fraction::Fraction;

// ===========================================================================
// const_gcd
// ===========================================================================

static void bench_const_gcd(ankerl::nanobench::Bench& b) {
    // Co-prime numbers (worst case for Euclidean algorithm)
    constexpr int a = 1234567;
    constexpr int b_val = 1234569;

    b.run("const_gcd(int, co-prime)", [&] {
        auto result = const_gcd(a, b_val);
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    // Large common divisor
    constexpr int c = 1000000;
    constexpr int d = 2000000;

    b.run("const_gcd(int, large common divisor)", [&] {
        auto result = const_gcd(c, d);
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    // int64_t
    constexpr int64_t e = 12345678901;
    constexpr int64_t f = 12345678903;

    b.run("const_gcd(int64_t, co-prime)", [&] {
        auto result = const_gcd(e, f);
        ankerl::nanobench::doNotOptimizeAway(result);
    });
}

// ===========================================================================
// Fraction construction
// ===========================================================================

static void bench_construction(ankerl::nanobench::Bench& b) {
    // from_raw (no normalization)
    b.run("Fraction<int>::from_raw", [&] {
        auto f = Fraction<int>::from_raw(3, 4);
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    // from (with normalization, no reduction needed)
    b.run("Fraction<int>::from (already reduced)", [&] {
        auto f = Fraction<int>::from(3, 4);
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    // from (with reduction)
    b.run("Fraction<int>::from (needs reduction)", [&] {
        auto f = Fraction<int>::from(100, 150);
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    // from negative denominator (triggers keep_denom_positive)
    b.run("Fraction<int>::from (neg denom)", [&] {
        auto f = Fraction<int>::from(3, -4);
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    // zero / one constants
    b.run("Fraction<int>::zero()", [&] {
        auto f = Fraction<int>::zero();
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    b.run("Fraction<int>::one()", [&] {
        auto f = Fraction<int>::one();
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    // int64_t construction
    b.run("Fraction<int64_t>::from (reduced)", [&] {
        auto f = Fraction<int64_t>::from(1000, 2000);
        ankerl::nanobench::doNotOptimizeAway(f);
    });
}

// ===========================================================================
// Arithmetic operations
// ===========================================================================

static void bench_arithmetic(ankerl::nanobench::Bench& b) {
    auto a = Fraction<int>::from(1, 2);
    auto b_val = Fraction<int>::from(1, 3);

    b.run("Fraction<int> addition 1/2 + 1/3", [&] {
        auto result = a + b_val;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    b.run("Fraction<int> subtraction 1/2 - 1/3", [&] {
        auto result = a - b_val;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    b.run("Fraction<int> multiplication 1/2 * 1/3", [&] {
        auto result = a * b_val;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    b.run("Fraction<int> division 1/2 / 1/3", [&] {
        auto result = a / b_val;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    // Same denominator (fast path)
    auto c = Fraction<int>::from(3, 7);
    auto d = Fraction<int>::from(5, 7);

    b.run("Fraction<int> addition (same denom) 3/7 + 5/7", [&] {
        auto result = c + d;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    // Scalar arithmetic
    b.run("Fraction<int> += scalar 1/2 += 3", [&] {
        auto f = a;
        f += 3;
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    // int64_t arithmetic
    auto a64 = Fraction<int64_t>::from(1, 2);
    auto b64 = Fraction<int64_t>::from(1, 3);

    b.run("Fraction<int64_t> addition 1/2 + 1/3", [&] {
        auto result = a64 + b64;
        ankerl::nanobench::doNotOptimizeAway(result);
    });
}

// ===========================================================================
// Comparison
// ===========================================================================

static void bench_comparison(ankerl::nanobench::Bench& b) {
    auto a = Fraction<int>::from(1, 2);
    auto b_val = Fraction<int>::from(1, 3);

    b.run("Fraction<int> operator<", [&] {
        auto result = a < b_val;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    b.run("Fraction<int> operator==", [&] {
        auto result = a == b_val;
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    // Same denominator comparison (fast path)
    auto c = Fraction<int>::from(3, 7);
    auto d = Fraction<int>::from(5, 7);

    b.run("Fraction<int> operator< (same denom)", [&] {
        auto result = c < d;
        ankerl::nanobench::doNotOptimizeAway(result);
    });
}

// ===========================================================================
// Cross product
// ===========================================================================

static void bench_cross_product(ankerl::nanobench::Bench& b) {
    auto a = Fraction<int>::from(1, 2);
    auto b_val = Fraction<int>::from(1, 3);

    b.run("Fraction<int> cross product 1/2 × 1/3", [&] {
        auto result = a.cross(b_val);
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    auto a64 = Fraction<int64_t>::from(5, 7);
    auto b64 = Fraction<int64_t>::from(3, 11);

    b.run("Fraction<int64_t> cross product 5/7 × 3/11", [&] {
        auto result = a64.cross(b64);
        ankerl::nanobench::doNotOptimizeAway(result);
    });
}

// ===========================================================================
// archimedes
// ===========================================================================

static void bench_archimedes(ankerl::nanobench::Bench& b) {
    auto sa = Fraction<int>::from(3, 1);
    auto sb = Fraction<int>::from(4, 1);
    auto sc = Fraction<int>::from(5, 1);

    b.run("archimedes<int> 3-4-5 triangle", [&] {
        auto result = archimedes(sa, sb, sc);
        ankerl::nanobench::doNotOptimizeAway(result);
    });

    auto sa64 = Fraction<int64_t>::from(5, 2);
    auto sb64 = Fraction<int64_t>::from(7, 3);
    auto sc64 = Fraction<int64_t>::from(8, 5);

    b.run("archimedes<int64_t> fractional sides", [&] {
        auto result = archimedes(sa64, sb64, sc64);
        ankerl::nanobench::doNotOptimizeAway(result);
    });
}

// ===========================================================================
// Normalization chain
// ===========================================================================

static void bench_normalization(ankerl::nanobench::Bench& b) {
    b.run("Fraction<int> normalize 150/200", [&] {
        auto f = Fraction<int>::from_raw(150, 200);
        f.normalize();
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    b.run("Fraction<int> reduce 1000/2500", [&] {
        auto f = Fraction<int>::from_raw(1000, 2500);
        f.reduce();
        ankerl::nanobench::doNotOptimizeAway(f);
    });

    b.run("Fraction<int> keep_denom_positive 3/-4", [&] {
        auto f = Fraction<int>::from_raw(3, -4);
        f.keep_denom_positive();
        ankerl::nanobench::doNotOptimizeAway(f);
    });
}

// ===========================================================================
// Main
// ===========================================================================

int main() {
    ankerl::nanobench::Bench bench;
    bench.title("fractions-simple benchmarks")
        .unit("iteration")
        .warmup(100)
        .minEpochIterations(5000);

    bench_const_gcd(bench);
    bench_construction(bench);
    bench_arithmetic(bench);
    bench_comparison(bench);
    bench_cross_product(bench);
    bench_archimedes(bench);
    bench_normalization(bench);
}
