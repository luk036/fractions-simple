#pragma once

#include <cstdlib>
#include <ostream>
#include <type_traits>

namespace fraction {

// ---------------------------------------------------------------------------
// const_abs: absolute value for signed integers
// ---------------------------------------------------------------------------

template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, int> = 0>
constexpr T const_abs(T val) noexcept {
    return val < 0 ? -val : val;
}

// ---------------------------------------------------------------------------
// const_gcd: greatest common divisor (Euclidean algorithm)
// ---------------------------------------------------------------------------

template <typename T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, int> = 0>
constexpr T const_gcd(T a, T b) noexcept {
    a = const_abs(a);
    b = const_abs(b);
    while (b != 0) {
        T t = b;
        b = a % b;
        a = t;
    }
    return a;
}

// ---------------------------------------------------------------------------
// Fraction<T> — a generic rational number type
// ---------------------------------------------------------------------------

template <typename T>
class Fraction {
    static_assert(std::is_integral_v<T> && std::is_signed_v<T>,
                  "Fraction<T> requires a signed integer type for T");

    T numer_;
    T denom_;

    // Private: normalised constructor used by from()
    constexpr Fraction(T num, T den, bool /*tag*/) noexcept
        : numer_(num), denom_(den) {
        normalize();
    }

public:
    // -----------------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------------

    /// Default: 0/1
    constexpr Fraction() noexcept
        : numer_(0), denom_(1) {}

    /// Raw (no normalisation) — public for advanced use
    constexpr static Fraction from_raw(T num, T den) noexcept {
        Fraction f;
        f.numer_ = num;
        f.denom_ = den;
        return f;
    }

    /// Normalised constructor
    constexpr static Fraction from(T num, T den) noexcept {
        return Fraction(num, den, true);
    }

    /// Zero constant: 0/1
    constexpr static Fraction zero() noexcept {
        return from_raw(0, 1);
    }

    /// One constant: 1/1
    constexpr static Fraction one() noexcept {
        return from_raw(1, 1);
    }

    // -----------------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------------

    constexpr const T& numer() const noexcept { return numer_; }
    constexpr const T& denom() const noexcept { return denom_; }

    // -----------------------------------------------------------------------
    // Predicates
    // -----------------------------------------------------------------------

    constexpr bool is_zero() const noexcept {
        return numer_ == 0 && denom_ != 0;
    }

    constexpr bool is_one() const noexcept {
        return numer_ == denom_ && numer_ != 0;
    }

    constexpr bool is_infinite() const noexcept {
        return numer_ != 0 && denom_ == 0;
    }

    constexpr bool is_nan() const noexcept {
        return numer_ == 0 && denom_ == 0;
    }

    constexpr bool is_positive() const noexcept {
        return numer_ > 0;
    }

    constexpr bool is_negative() const noexcept {
        return numer_ < 0;
    }

    // -----------------------------------------------------------------------
    // Mutators
    // -----------------------------------------------------------------------

    constexpr void set_zero() noexcept {
        numer_ = 0;
        denom_ = 1;
    }

    constexpr void set_one() noexcept {
        numer_ = 1;
        denom_ = 1;
    }

    constexpr void set_infinite() noexcept {
        numer_ = 1;
        denom_ = 0;
    }

    constexpr void set_nan() noexcept {
        numer_ = 0;
        denom_ = 0;
    }

    /// Keep denominator positive by flipping signs if needed
    constexpr void keep_denom_positive() noexcept {
        if (denom_ < 0) {
            numer_ = -numer_;
            denom_ = -denom_;
        }
    }

    /// Reduce fraction by dividing numerator and denominator by their GCD.
    /// Returns the GCD that was divided out.
    constexpr T reduce() noexcept {
        T g = const_gcd(numer_, denom_);
        if (g != 0 && g != 1) {
            numer_ /= g;
            denom_ /= g;
        }
        return g;
    }

    /// Normalise: keep denominator positive then reduce.
    /// Returns the GCD that was divided out.
    constexpr T normalize() noexcept {
        keep_denom_positive();
        return reduce();
    }

    /// Swap numerator and denominator (the fraction is inverted).
    constexpr void reciprocal() noexcept {
        T tmp = numer_;
        numer_ = denom_;
        denom_ = tmp;
        keep_denom_positive();
    }

    // -----------------------------------------------------------------------
    // Computed properties
    // -----------------------------------------------------------------------

    constexpr Fraction abs() const noexcept {
        return is_negative() ? -*this : *this;
    }

    constexpr Fraction signum() const noexcept {
        if (is_positive()) return one();
        if (is_zero())     return zero();
        return -one();
    }

    /// Cross product:  a*d - b*c   (where self = a/b, rhs = c/d)
    constexpr T cross(const Fraction& rhs) const noexcept {
        return numer_ * rhs.denom_ - denom_ * rhs.numer_;
    }

    /// Multiplicative inverse (reciprocal)
    constexpr Fraction inv() const noexcept {
        Fraction r = *this;
        r.reciprocal();
        return r;
    }

    // -----------------------------------------------------------------------
    // Unary operators
    // -----------------------------------------------------------------------

    constexpr Fraction operator-() const noexcept {
        return from_raw(-numer_, denom_);
    }

    // -----------------------------------------------------------------------
    // Compound assignment (Fraction)
    // -----------------------------------------------------------------------

    constexpr Fraction& operator+=(const Fraction& rhs) noexcept {
        if (is_nan() || rhs.is_nan()) {
            set_nan();
            return *this;
        }
        if (is_infinite() && rhs.is_infinite()) {
            if (is_negative() != rhs.is_negative())
                set_nan();
            return *this;
        }
        if (is_infinite()) return *this;
        if (rhs.is_infinite()) {
            *this = rhs;
            return *this;
        }

        if (denom_ == rhs.denom_) {
            numer_ += rhs.numer_;
            reduce();
            return *this;
        }

        // a/b + c/d = (a*d + b*c) / (b*d)
        Fraction tmp = rhs;
        // Cross-reduce: swap self.denom with tmp.numer
        T s = denom_;
        denom_ = tmp.numer_;
        tmp.numer_ = s;

        T common_n = reduce();
        T common_d = tmp.reduce();

        // Swap back
        s = denom_;
        denom_ = tmp.numer_;
        tmp.numer_ = s;

        numer_ = numer_ * tmp.denom_ + denom_ * tmp.numer_;
        denom_ *= tmp.denom_;

        // Cross-cancel: swap self.denom with common_d
        s = denom_;
        denom_ = common_d;
        common_d = s;

        reduce();
        numer_ *= common_n;
        denom_ *= common_d;
        reduce();

        return *this;
    }

    constexpr Fraction& operator-=(const Fraction& rhs) noexcept {
        if (is_nan() || rhs.is_nan()) {
            set_nan();
            return *this;
        }
        if (is_infinite() && rhs.is_infinite()) {
            set_nan();
            return *this;
        }
        if (is_infinite()) return *this;
        if (rhs.is_infinite()) {
            *this = -rhs;
            return *this;
        }

        if (denom_ == rhs.denom_) {
            numer_ -= rhs.numer_;
            reduce();
            return *this;
        }

        Fraction tmp = rhs;
        // Cross-reduce: swap self.denom with tmp.numer
        T s = denom_;
        denom_ = tmp.numer_;
        tmp.numer_ = s;

        T common_n = reduce();
        T common_d = tmp.reduce();

        // Swap back
        s = denom_;
        denom_ = tmp.numer_;
        tmp.numer_ = s;

        numer_ = cross(tmp); // a*d - b*c
        denom_ *= tmp.denom_;

        // Cross-cancel: swap self.denom with common_d
        s = denom_;
        denom_ = common_d;
        common_d = s;

        reduce();
        numer_ *= common_n;
        denom_ *= common_d;
        reduce();

        return *this;
    }

    constexpr Fraction& operator*=(const Fraction& rhs) noexcept {
        if (is_nan() || rhs.is_nan()) {
            set_nan();
            return *this;
        }
        if ((is_infinite() && rhs.is_zero()) || (is_zero() && rhs.is_infinite())) {
            set_nan();
            return *this;
        }
        if (is_zero() || rhs.is_zero()) {
            set_zero();
            return *this;
        }
        if (is_infinite() || rhs.is_infinite()) {
            bool neg = is_negative() != rhs.is_negative();
            numer_ = neg ? -1 : 1;
            denom_ = 0;
            return *this;
        }

        // Cross-reduce before multiplying
        Fraction r = rhs;
        T t = numer_;
        numer_ = r.numer_;
        r.numer_ = t;

        reduce();
        r.reduce();

        numer_ *= r.numer_;
        denom_ *= r.denom_;

        return *this;
    }

    constexpr Fraction& operator/=(const Fraction& rhs) noexcept {
        if (is_nan() || rhs.is_nan()) {
            set_nan();
            return *this;
        }
        if (is_infinite() && rhs.is_infinite()) {
            set_nan();
            return *this;
        }
        if (is_zero() && rhs.is_zero()) {
            set_nan();
            return *this;
        }
        if (rhs.is_zero()) {
            numer_ = is_negative() ? -1 : 1;
            denom_ = 0;
            return *this;
        }
        if (is_infinite()) return *this;
        if (rhs.is_infinite()) {
            set_zero();
            return *this;
        }

        // a/b ÷ c/d = a*d / b*c
        Fraction r = rhs;
        T t = denom_;
        denom_ = r.numer_;
        r.numer_ = t;

        normalize();
        r.reduce();

        numer_ *= r.denom_;
        denom_ *= r.numer_;

        return *this;
    }

    // -----------------------------------------------------------------------
    // Compound assignment (scalar T)
    // -----------------------------------------------------------------------

    constexpr Fraction& operator+=(T rhs) noexcept {
        if (denom_ == 1) {
            numer_ += rhs;
            reduce();
            return *this;
        }
        T t = denom_;
        denom_ = rhs;
        T common_n = reduce();
        denom_ = t;
        numer_ += denom_ * rhs;
        reduce();
        numer_ *= common_n;
        return *this;
    }

    constexpr Fraction& operator-=(T rhs) noexcept {
        if (denom_ == 1) {
            numer_ -= rhs;
            reduce();
            return *this;
        }
        T t = denom_;
        denom_ = rhs;
        T common_n = reduce();
        denom_ = t;
        numer_ -= denom_ * rhs;
        reduce();
        numer_ *= common_n;
        return *this;
    }

    constexpr Fraction& operator*=(T rhs) noexcept {
        T t = numer_;
        numer_ = rhs;
        reduce();
        numer_ *= t;
        return *this;
    }

    constexpr Fraction& operator/=(T rhs) noexcept {
        T t = denom_;
        denom_ = rhs;
        normalize();
        denom_ *= t;
        return *this;
    }

    // -----------------------------------------------------------------------
    // Equality / ordering
    // -----------------------------------------------------------------------

    constexpr bool operator==(const Fraction& rhs) const noexcept {
        return numer_ == rhs.numer_ && denom_ == rhs.denom_;
    }

    constexpr bool operator!=(const Fraction& rhs) const noexcept {
        return !(*this == rhs);
    }

    constexpr bool operator<(const Fraction& rhs) const noexcept {
        if (denom_ == rhs.denom_)
            return numer_ < rhs.numer_;
        // Cross-multiply with reduction
        Fraction lhs_f = from_raw(numer_, rhs.numer_);
        Fraction rhs_f = from_raw(denom_, rhs.denom_);
        lhs_f.reduce();
        rhs_f.reduce();
        return lhs_f.numer_ * rhs_f.denom_ < lhs_f.denom_ * rhs_f.numer_;
    }

    constexpr bool operator<=(const Fraction& rhs) const noexcept {
        return !(rhs < *this);
    }

    constexpr bool operator>(const Fraction& rhs) const noexcept {
        return rhs < *this;
    }

    constexpr bool operator>=(const Fraction& rhs) const noexcept {
        return !(*this < rhs);
    }

    // -----------------------------------------------------------------------
    // Comparison with scalar
    // -----------------------------------------------------------------------

    constexpr bool operator==(T rhs) const noexcept {
        return denom_ == 1 && numer_ == rhs;
    }

    constexpr bool operator!=(T rhs) const noexcept {
        return !(*this == rhs);
    }

    constexpr bool operator<(T rhs) const noexcept {
        if (denom_ == 1 || rhs == 0)
            return numer_ < rhs;
        Fraction f = from_raw(numer_, rhs);
        f.reduce();
        return f.numer_ < f.denom_ * denom_;
    }

    constexpr bool operator<=(T rhs) const noexcept {
        return !(rhs < *this);
    }

    constexpr bool operator>(T rhs) const noexcept {
        return rhs < *this;
    }

    constexpr bool operator>=(T rhs) const noexcept {
        return !(*this < rhs);
    }

    // -----------------------------------------------------------------------
    // Friend arithmetic operators (Fraction)
    // -----------------------------------------------------------------------

    friend constexpr Fraction operator+(Fraction lhs, const Fraction& rhs) noexcept {
        lhs += rhs;
        return lhs;
    }

    friend constexpr Fraction operator-(Fraction lhs, const Fraction& rhs) noexcept {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr Fraction operator*(Fraction lhs, const Fraction& rhs) noexcept {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr Fraction operator/(Fraction lhs, const Fraction& rhs) noexcept {
        lhs /= rhs;
        return lhs;
    }

    // -----------------------------------------------------------------------
    // Friend arithmetic operators (scalar)
    // -----------------------------------------------------------------------

    friend constexpr Fraction operator+(Fraction lhs, T rhs) noexcept {
        lhs += rhs;
        return lhs;
    }

    friend constexpr Fraction operator-(Fraction lhs, T rhs) noexcept {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr Fraction operator*(Fraction lhs, T rhs) noexcept {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr Fraction operator/(Fraction lhs, T rhs) noexcept {
        lhs /= rhs;
        return lhs;
    }

    // Scalar op Fraction
    friend constexpr Fraction operator+(T lhs, const Fraction& rhs) noexcept {
        return Fraction::from(lhs, 1) + rhs;
    }

    friend constexpr Fraction operator-(T lhs, const Fraction& rhs) noexcept {
        return Fraction::from(lhs, 1) - rhs;
    }

    friend constexpr Fraction operator*(T lhs, const Fraction& rhs) noexcept {
        return Fraction::from(lhs, 1) * rhs;
    }

    friend constexpr Fraction operator/(T lhs, const Fraction& rhs) noexcept {
        return Fraction::from(lhs, 1) / rhs;
    }

    // Scalar comparison (Scalar op Fraction)
    friend constexpr bool operator==(T lhs, const Fraction& rhs) noexcept {
        return rhs.denom_ == 1 && rhs.numer_ == lhs;
    }

    friend constexpr bool operator!=(T lhs, const Fraction& rhs) noexcept {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(T lhs, const Fraction& rhs) noexcept {
        if (rhs.denom_ == 1 || lhs == 0)
            return lhs < rhs.numer_;
        Fraction f = Fraction::from_raw(lhs, 1);
        return f < rhs;
    }

    friend constexpr bool operator<=(T lhs, const Fraction& rhs) noexcept {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>(T lhs, const Fraction& rhs) noexcept {
        return rhs < lhs;
    }

    friend constexpr bool operator>=(T lhs, const Fraction& rhs) noexcept {
        return !(lhs < rhs);
    }

    // -----------------------------------------------------------------------
    // Streaming
    // -----------------------------------------------------------------------

    friend std::ostream& operator<<(std::ostream& os, const Fraction& f) {
        if (f.is_nan()) {
            os << "nan";
        } else if (f.is_infinite()) {
            os << (f.is_negative() ? "-inf" : "inf");
        } else if (f.denom_ == 1) {
            os << f.numer_;
        } else {
            os << f.numer_ << "/" << f.denom_;
        }
        return os;
    }
};

// ---------------------------------------------------------------------------
// archimedes: triangle area using Archimedes' formula
// ---------------------------------------------------------------------------

template <typename T>
constexpr T archimedes(const T& side_a, const T& side_b, const T& side_c) {
    T temp_sum = side_a + side_b - side_c;
    return T::from_raw(4, 1) * side_a * side_b - temp_sum * temp_sum;
}

} // namespace fraction
