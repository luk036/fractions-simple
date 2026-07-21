#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fraction/fraction.hpp>
#include <sstream>

using fraction::Fraction;
using fraction::const_abs;
using fraction::const_gcd;
using fraction::archimedes;

// ===========================================================================
// const_abs / const_gcd
// ===========================================================================

TEST_CASE("const_abs") {
    CHECK(const_abs(-10) == 10);
    CHECK(const_abs(10) == 10);
    CHECK(const_abs(0) == 0);
}

TEST_CASE("const_gcd") {
    CHECK(const_gcd(30, -40) == 10);
    CHECK(const_gcd(30, 40) == 10);
    CHECK(const_gcd(0, 5) == 5);
    CHECK(const_gcd(5, 0) == 5);
    CHECK(const_gcd(-30, -40) == 10);
    CHECK(const_gcd(0, 0) == 0);
    CHECK(const_gcd(1, 0) == 1);
    CHECK(const_gcd(0, 1) == 1);
    CHECK(const_gcd(-1, 0) == 1);
    CHECK(const_gcd(0, -1) == 1);
    CHECK(const_gcd(10, 20) == 10);
    CHECK(const_gcd(-10, 20) == 10);
    CHECK(const_gcd(10, -20) == 10);
    CHECK(const_gcd(-10, -20) == 10);
}

// ===========================================================================
// Construction / Normalization
// ===========================================================================

TEST_CASE("fraction construction") {
    auto f = Fraction<int>::zero();
    CHECK(f.numer() == 0);
    CHECK(f.denom() == 1);

    auto fi = Fraction<int>::from(5, 1);
    CHECK(fi.numer() == 5);
    CHECK(fi.denom() == 1);

    auto fr = Fraction<int>::from_raw(3, 4);
    CHECK(fr.numer() == 3);
    CHECK(fr.denom() == 4);

    auto fn = Fraction<int>::from(2, 4);
    CHECK(fn.numer() == 1);
    CHECK(fn.denom() == 2);

    auto fneg = Fraction<int>::from(3, -4);
    CHECK(fneg.numer() == -3);
    CHECK(fneg.denom() == 4);
}

TEST_CASE("fraction normalization") {
    auto f1 = Fraction<int>::from_raw(4, 6);
    CHECK(f1.reduce() == 2);
    CHECK(f1.numer() == 2);
    CHECK(f1.denom() == 3);

    auto f2 = Fraction<int>::from_raw(3, -4);
    f2.keep_denom_positive();
    CHECK(f2.numer() == -3);
    CHECK(f2.denom() == 4);

    auto f3 = Fraction<int>::from_raw(10, -5);
    f3.normalize();
    CHECK(f3.numer() == -2);
    CHECK(f3.denom() == 1);
}

// ===========================================================================
// Arithmetic
// ===========================================================================

TEST_CASE("fraction arithmetic") {
    auto f1 = Fraction<int>::from(1, 2);
    auto f2 = Fraction<int>::from(1, 3);

    CHECK(f1 + f2 == Fraction<int>::from(5, 6));
    CHECK(f1 + 1 == Fraction<int>::from(3, 2));
    CHECK(f1 - f2 == Fraction<int>::from(1, 6));
    CHECK(f1 - 1 == Fraction<int>::from(-1, 2));
    CHECK(f1 * f2 == Fraction<int>::from(1, 6));
    CHECK(f1 * 2 == Fraction<int>::from(1, 1));
    CHECK(f1 / f2 == Fraction<int>::from(3, 2));
    CHECK(f1 / 2 == Fraction<int>::from(1, 4));
    CHECK(-f1 == Fraction<int>::from(-1, 2));
}

TEST_CASE("fraction compound assign") {
    auto f = Fraction<int>::from(1, 2);

    f += Fraction<int>::from(1, 3);
    CHECK(f == Fraction<int>::from(5, 6));

    f -= Fraction<int>::from(1, 6);
    CHECK(f == Fraction<int>::from(2, 3));

    f *= Fraction<int>::from(3, 2);
    CHECK(f == Fraction<int>::from(1, 1));

    f /= Fraction<int>::from(2, 1);
    CHECK(f == Fraction<int>::from(1, 2));

    f += 1;
    CHECK(f == Fraction<int>::from(3, 2));

    f -= 1;
    CHECK(f == Fraction<int>::from(1, 2));

    f *= 2;
    CHECK(f == Fraction<int>::from(1, 1));

    f /= 2;
    CHECK(f == Fraction<int>::from(1, 2));
}

// ===========================================================================
// Comparison
// ===========================================================================

TEST_CASE("fraction comparison") {
    auto f1 = Fraction<int>::from(1, 2);
    auto f2 = Fraction<int>::from(1, 3);
    auto f3 = Fraction<int>::from(2, 4);

    CHECK(f1 == f3);
    CHECK(f1 != f2);
    CHECK(Fraction<int>::from(3, 1) == 3);
    CHECK(3 == Fraction<int>::from(3, 1));

    CHECK(f1 > f2);
    CHECK(f2 < f1);
    CHECK(f1 >= f3);
    CHECK(f1 <= f3);
    CHECK(Fraction<int>::from(3, 2) > 1);
    CHECK(1 < Fraction<int>::from(3, 2));
}

TEST_CASE("equal denominator comparison") {
    auto f1 = Fraction<int>::from(1, 4);
    auto f2 = Fraction<int>::from(2, 4);
    CHECK(f1 < f2);

    auto f3 = Fraction<int>::from(3, 4);
    auto f4 = Fraction<int>::from(3, 4);
    CHECK(f3 == f4);

    auto f5 = Fraction<int>::from(5, 4);
    CHECK(f5 > f4);
}

// ===========================================================================
// Special values
// ===========================================================================

TEST_CASE("fraction special values") {
    auto zero = Fraction<int>::from(0, 1);
    CHECK(zero.is_zero());
    CHECK_FALSE(zero.is_infinite());
    CHECK_FALSE(zero.is_nan());

    auto fz = Fraction<int>::from(1, 1);
    fz.set_zero();
    CHECK(fz.is_zero());

    auto inf = Fraction<int>::from_raw(1, 0);
    CHECK(inf.is_infinite());
    CHECK_FALSE(inf.is_nan());

    auto nan = Fraction<int>::from_raw(0, 0);
    CHECK(nan.is_nan());
    CHECK_FALSE(nan.is_infinite());
}

TEST_CASE("more special values") {
    auto zero = Fraction<int>::from(0, 1);
    auto inf = Fraction<int>::from_raw(1, 0);
    auto neg_inf = Fraction<int>::from_raw(-1, 0);
    auto nan = Fraction<int>::from_raw(0, 0);

    CHECK(Fraction<int>::from(1, 1) / zero == inf);
    CHECK(Fraction<int>::from(-1, 1) / zero == neg_inf);

    CHECK((nan + inf).is_nan());
    CHECK((nan - inf).is_nan());
    CHECK((nan * inf).is_nan());
    CHECK((nan / inf).is_nan());
    CHECK((inf + nan).is_nan());
    CHECK((inf - nan).is_nan());
    CHECK((inf * nan).is_nan());
    CHECK((inf / nan).is_nan());
}

TEST_CASE("extra special values") {
    auto inf = Fraction<int>::from_raw(1, 0);
    auto neg_inf = Fraction<int>::from_raw(-1, 0);

    CHECK((inf + neg_inf).is_nan());
    CHECK((neg_inf + inf).is_nan());
    CHECK((inf / inf).is_nan());
    CHECK((inf / neg_inf).is_nan());
    CHECK((neg_inf / inf).is_nan());
    CHECK((neg_inf / neg_inf).is_nan());
    CHECK((inf * Fraction<int>::from(0, 1)).is_nan());
    CHECK((neg_inf * Fraction<int>::from(0, 1)).is_nan());
}

TEST_CASE("zero edge cases") {
    auto zero = Fraction<int>::from(0, 5);
    CHECK(zero.is_zero());
    CHECK_FALSE(zero.is_nan());
    CHECK_FALSE(zero.is_infinite());

    auto nan = Fraction<int>::from(0, 0);
    CHECK_FALSE(nan.is_zero());
}

// ===========================================================================
// Division / multiplication special cases
// ===========================================================================

TEST_CASE("division by zero") {
    auto zero = Fraction<int>::from(0, 1);
    auto f = Fraction<int>::from(0, 1);
    f /= zero;
    CHECK(f.is_nan());
}

TEST_CASE("infinite divided by infinite") {
    auto f1 = Fraction<int>::from_raw(1, 0);
    auto f2 = Fraction<int>::from_raw(1, 0);
    f1 /= f2;
    CHECK(f1.is_nan());
}

TEST_CASE("infinite divided by zero") {
    auto f1 = Fraction<int>::from_raw(1, 0);
    f1 /= Fraction<int>::from(0, 1);
    CHECK(f1.is_infinite());
    CHECK(f1.numer() > 0);

    auto f2 = Fraction<int>::from_raw(-1, 0);
    f2 /= Fraction<int>::from(0, 1);
    CHECK(f2.is_infinite());
    CHECK(f2.numer() < 0);
}

TEST_CASE("self infinite division") {
    auto inf = Fraction<int>::from_raw(1, 0);
    inf /= Fraction<int>::from(2, 1);
    CHECK(inf.is_infinite());
}

TEST_CASE("other infinite division") {
    auto f = Fraction<int>::from(1, 1);
    auto result = f / Fraction<int>::from_raw(1, 0);
    CHECK(result.is_zero());
}

TEST_CASE("infinite times zero") {
    auto f1 = Fraction<int>::from_raw(1, 0);
    f1 *= Fraction<int>::from(0, 1);
    CHECK(f1.is_nan());

    auto f2 = Fraction<int>::from(0, 1);
    f2 *= Fraction<int>::from_raw(1, 0);
    CHECK(f2.is_nan());
}

TEST_CASE("zero multiplication") {
    auto f = Fraction<int>::from(0, 1);
    f *= Fraction<int>::from(5, 6);
    CHECK(f.is_zero());

    auto g = Fraction<int>::from(1, 2);
    g *= Fraction<int>::from(0, 1);
    CHECK(g.is_zero());
}

TEST_CASE("infinite multiplication") {
    auto f1 = Fraction<int>::from_raw(1, 0);
    f1 *= Fraction<int>::from_raw(1, 0);
    CHECK(f1.is_infinite());

    auto f2 = Fraction<int>::from_raw(1, 0);
    f2 *= Fraction<int>::from_raw(-1, 0);
    CHECK(f2.is_infinite());
    CHECK(f2.numer() < 0);
}

// ===========================================================================
// Add / Sub special cases
// ===========================================================================

TEST_CASE("add same denominator") {
    auto f1 = Fraction<int>::from(1, 4);
    auto f2 = Fraction<int>::from(2, 4);
    f1 += f2;
    CHECK(f1 == Fraction<int>::from(3, 4));

    auto f3 = Fraction<int>::from(5, 1);
    f3 += Fraction<int>::from(3, 1);
    CHECK(f3 == Fraction<int>::from(8, 1));
}

TEST_CASE("add self infinity") {
    auto inf = Fraction<int>::from_raw(1, 0);
    inf += Fraction<int>::from(1, 1);
    CHECK(inf.is_infinite());

    auto neg_inf = Fraction<int>::from_raw(-1, 0);
    neg_inf += Fraction<int>::from(1, 1);
    CHECK(neg_inf.is_infinite());
    CHECK(neg_inf.numer() < 0);
}

TEST_CASE("add other infinity") {
    auto f = Fraction<int>::from(1, 2);
    f += Fraction<int>::from_raw(1, 0);
    CHECK(f.is_infinite());

    auto f2 = Fraction<int>::from(1, 2);
    f2 += Fraction<int>::from_raw(-1, 0);
    CHECK(f2.is_infinite());
    CHECK(f2.numer() < 0);
}

TEST_CASE("add both infinity opposite sign") {
    auto inf = Fraction<int>::from_raw(1, 0);
    inf += Fraction<int>::from_raw(-1, 0);
    CHECK(inf.is_nan());

    auto neg_inf = Fraction<int>::from_raw(-1, 0);
    neg_inf += Fraction<int>::from_raw(1, 0);
    CHECK(neg_inf.is_nan());
}

TEST_CASE("sub both infinity") {
    auto inf = Fraction<int>::from_raw(1, 0);
    inf -= Fraction<int>::from_raw(1, 0);
    CHECK(inf.is_nan());
}

TEST_CASE("sub self infinity") {
    auto inf = Fraction<int>::from_raw(1, 0);
    inf -= Fraction<int>::from(1, 1);
    CHECK(inf.is_infinite());
}

TEST_CASE("sub other infinity") {
    auto f = Fraction<int>::from(1, 2);
    f -= Fraction<int>::from_raw(1, 0);
    CHECK(f.is_infinite());
    CHECK(f.numer() < 0);

    auto f2 = Fraction<int>::from(1, 2);
    f2 -= Fraction<int>::from_raw(1, 0);
    CHECK(f2.is_infinite());
}

TEST_CASE("sub same denominator") {
    auto f1 = Fraction<int>::from(3, 4);
    auto f2 = Fraction<int>::from(1, 4);
    f1 -= f2;
    CHECK(f1 == Fraction<int>::from(1, 2));
}

// ===========================================================================
// Cross product
// ===========================================================================

TEST_CASE("cross product") {
    auto f1 = Fraction<int>::from(3, 4);
    auto f2 = Fraction<int>::from(-5, 6);
    CHECK(f1.cross(f2) == 38);

    CHECK(Fraction<int>::from(1, 2).cross(Fraction<int>::from(3, 4)) == -2);
    CHECK(Fraction<int>::from(1, 2).cross(Fraction<int>::from(0, 1)) == 1);
    CHECK(Fraction<int>::from(1, 2).cross(Fraction<int>::from(-1, 2)) == 4);
}

// ===========================================================================
// Abs / neg / reciprocal
// ===========================================================================

TEST_CASE("absolute value") {
    CHECK(Fraction<int>::from(-3, 4).abs() == Fraction<int>::from(3, 4));
    CHECK(Fraction<int>::from(3, -4).abs() == Fraction<int>::from(3, 4));
}

TEST_CASE("negation") {
    CHECK(-Fraction<int>::from(-3, 4) == Fraction<int>::from(-3, 4).abs());
}

TEST_CASE("reciprocal") {
    CHECK(Fraction<int>::from(2, 1).inv() == Fraction<int>::from(1, 2));
    CHECK(Fraction<int>::from(1, 1).inv() == Fraction<int>::from(1, 1));
    CHECK(Fraction<int>::from(10, 5).inv() == Fraction<int>::from(1, 2));
}

// ===========================================================================
// i8 type
// ===========================================================================

TEST_CASE("i8 construction") {
    CHECK(Fraction<int8_t>::from(1, 2) == Fraction<int8_t>::from(1, 2));
    CHECK(Fraction<int8_t>::from_raw(3, 4).numer() == 3);
    CHECK(Fraction<int8_t>::from_raw(3, 4).denom() == 4);
    CHECK(Fraction<int8_t>::zero().is_zero());
    CHECK(Fraction<int8_t>::one().is_one());
}

TEST_CASE("i8 special values") {
    CHECK(Fraction<int8_t>::from(0, 1).is_zero());
    CHECK_FALSE(Fraction<int8_t>::from(0, 1).is_infinite());
    CHECK_FALSE(Fraction<int8_t>::from(0, 1).is_nan());
    CHECK(Fraction<int8_t>::from_raw(1, 0).is_infinite());
    CHECK(Fraction<int8_t>::from_raw(-1, 0).is_infinite());
    CHECK(Fraction<int8_t>::from_raw(0, 0).is_nan());
}

TEST_CASE("i8 setters") {
    auto f = Fraction<int8_t>::from(1, 2);
    f.set_zero();
    CHECK(f.is_zero());
    f.set_one();
    CHECK(f.is_one());
    f.set_infinite();
    CHECK(f.is_infinite());
    f.set_nan();
    CHECK(f.is_nan());
}

TEST_CASE("i8 signum and predicates") {
    auto pos = Fraction<int8_t>::from(3, 4);
    CHECK(pos.is_positive());
    CHECK_FALSE(pos.is_negative());
    CHECK(pos.signum() == Fraction<int8_t>::one());

    auto neg = Fraction<int8_t>::from(-3, 4);
    CHECK_FALSE(neg.is_positive());
    CHECK(neg.is_negative());
    CHECK(neg.signum() == -Fraction<int8_t>::one());

    auto zero = Fraction<int8_t>::from(0, 1);
    CHECK(zero.signum() == Fraction<int8_t>::zero());
}

TEST_CASE("i8 arithmetic") {
    auto f1 = Fraction<int8_t>::from(1, 2);
    auto f2 = Fraction<int8_t>::from(1, 3);

    CHECK(f1 + f2 == Fraction<int8_t>::from(5, 6));
    CHECK(f1 - f2 == Fraction<int8_t>::from(1, 6));
    CHECK(f1 * f2 == Fraction<int8_t>::from(1, 6));
    CHECK(f1 / f2 == Fraction<int8_t>::from(3, 2));
    CHECK(-f1 == Fraction<int8_t>::from(-1, 2));
    CHECK(f1.abs() == Fraction<int8_t>::from(1, 2));
}

TEST_CASE("i8 comparison") {
    CHECK(Fraction<int8_t>::from(1, 2) > Fraction<int8_t>::from(1, 3));
    CHECK(Fraction<int8_t>::from(1, 2) == Fraction<int8_t>::from(2, 4));
    CHECK(Fraction<int8_t>::from(3, 1) == 3);
    CHECK(3 == Fraction<int8_t>::from(3, 1));
    CHECK(Fraction<int8_t>::from(3, 1) > 1);
    CHECK(1 < Fraction<int8_t>::from(3, 1));
}

TEST_CASE("i8 compound assign") {
    auto f = Fraction<int8_t>::from(1, 2);
    f += Fraction<int8_t>::from(1, 3);
    CHECK(f == Fraction<int8_t>::from(5, 6));
    f -= Fraction<int8_t>::from(1, 6);
    CHECK(f == Fraction<int8_t>::from(2, 3));
    f *= Fraction<int8_t>::from(3, 2);
    CHECK(f == Fraction<int8_t>::from(1, 1));
    f /= Fraction<int8_t>::from(2, 1);
    CHECK(f == Fraction<int8_t>::from(1, 2));
}

TEST_CASE("i8 scalar assign") {
    auto f = Fraction<int8_t>::from(1, 2);
    f += 1;
    CHECK(f == Fraction<int8_t>::from(3, 2));
    f -= 1;
    CHECK(f == Fraction<int8_t>::from(1, 2));
    f *= 2;
    CHECK(f == Fraction<int8_t>::from(1, 1));
    f /= 2;
    CHECK(f == Fraction<int8_t>::from(1, 2));
}

// ===========================================================================
// i64 type
// ===========================================================================

TEST_CASE("i64 construction") {
    CHECK(Fraction<int64_t>::from(1, 2) == Fraction<int64_t>::from(1, 2));
    CHECK(Fraction<int64_t>::from_raw(3, 4).numer() == 3);
    CHECK(Fraction<int64_t>::from_raw(3, 4).denom() == 4);
}

TEST_CASE("i64 special values") {
    CHECK(Fraction<int64_t>::from_raw(1, 0).is_infinite());
    CHECK(Fraction<int64_t>::from_raw(-1, 0).is_infinite());
    CHECK(Fraction<int64_t>::from_raw(0, 0).is_nan());
}

// ===========================================================================
// Streaming
// ===========================================================================

TEST_CASE("stream output") {
    std::ostringstream oss;

    oss << Fraction<int>::from(3, 4);
    CHECK(oss.str() == "3/4");

    oss.str("");
    oss << Fraction<int>::from(5, 1);
    CHECK(oss.str() == "5");

    oss.str("");
    oss << Fraction<int>::from_raw(0, 0);
    CHECK(oss.str() == "nan");

    oss.str("");
    oss << Fraction<int>::from_raw(1, 0);
    CHECK(oss.str() == "inf");

    oss.str("");
    oss << Fraction<int>::from_raw(-1, 0);
    CHECK(oss.str() == "-inf");
}

// ===========================================================================
// Archimedes
// ===========================================================================

TEST_CASE("archimedes") {
    auto side_a = Fraction<int>::from(1, 2);
    auto side_b = Fraction<int>::from(1, 4);
    auto side_c = Fraction<int>::from(1, 6);
    auto result = archimedes(side_a, side_b, side_c);
    CHECK(result == Fraction<int>::from(23, 144));
}

TEST_CASE("archimedes i64") {
    auto side_a = Fraction<int64_t>::from(1, 2);
    auto side_b = Fraction<int64_t>::from(1, 4);
    auto side_c = Fraction<int64_t>::from(1, 6);
    auto result = archimedes(side_a, side_b, side_c);
    CHECK(result == Fraction<int64_t>::from(23, 144));
}
