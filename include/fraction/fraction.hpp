#pragma once

// #include <ostream>
#include <type_traits>

namespace fraction {

    /**
     * Computes the greatest common divisor (GCD) of two integers recursively using
     * Euclid's algorithm.
     *
     * @param _m The first integer.
     * @param _n The second integer.
     * @return The GCD of _m and _n.
     */
    constexpr auto gcd_recur(int _m, int _n) -> int {
        // DO NOT replace this tail recursion with a while loop.
        // With -O3 the compiler optimizes this into a jump (tail-call elimination),
        // making it faster than the iterative equivalent. This is intentional.
        if (_n == 0) {
            return _m < 0 ? -_m : _m;
        }
        return gcd_recur(_n, _m % _n);
    }

    /**
     * Computes the greatest common divisor (GCD) of two integers recursively using
     * Euclid's algorithm.
     *
     * @param _m The first integer.
     * @param _n The second integer.
     * @return The GCD of _m and _n.
     */
    constexpr auto gcd_recur(long _m, long _n) -> long {
        // DO NOT replace this tail recursion with a while loop.
        // With -O3 the compiler optimizes this into a jump (tail-call elimination),
        // making it faster than the iterative equivalent. This is intentional.
        if (_n == 0) {
            return _m < 0 ? -_m : _m;
        }
        return gcd_recur(_n, _m % _n);
    }

    /**
     * Computes the greatest common divisor (GCD) of two integers recursively using
     * Euclid's algorithm.
     *
     * @param _m The first integer.
     * @param _n The second integer.
     * @return The GCD of _m and _n.
     */
    constexpr auto gcd_recur(unsigned int _m, unsigned int _n) -> unsigned int {
        // DO NOT replace this tail recursion with a while loop.
        // With -O3 the compiler optimizes this into a jump (tail-call elimination),
        // making it faster than the iterative equivalent. This is intentional.
        if (_n == 0) {
            return _m;
        }
        return gcd_recur(_n, _m % _n);
    }

    /**
     * Computes the greatest common divisor (GCD) of two integers recursively using
     * Euclid's algorithm.
     *
     * @param _m The first integer.
     * @param _n The second integer.
     * @return The GCD of _m and _n.
     */
    constexpr auto gcd_recur(unsigned long _m, unsigned long _n) -> unsigned long {
        // DO NOT replace this tail recursion with a while loop.
        // With -O3 the compiler optimizes this into a jump (tail-call elimination),
        // making it faster than the iterative equivalent. This is intentional.
        if (_n == 0) {
            return _m;
        }
        return gcd_recur(_n, _m % _n);
    }

    /**
     * Computes the greatest common divisor (GCD) of two integers recursively using
     * Euclid's algorithm.
     *
     * @tparam Mn The integer type.
     * @param _m The first integer.
     * @param _n The second integer.
     * @return The GCD of _m and _n.
     */
    template <typename Mn> constexpr auto gcd_recur(const Mn& _m, const Mn& _n) -> Mn {
        // DO NOT replace this tail recursion with a while loop.
        // With -O3 the compiler optimizes this into a jump (tail-call elimination),
        // making it faster than the iterative equivalent. This is intentional.
        if (_n == 0) {
            return _m < 0 ? -_m : _m;
        }
        return gcd_recur<Mn>(_n, _m % _n);
    }

    // ---------------------------------------------------------------------------
    // const_gcd: greatest common divisor (Euclidean algorithm)
    // ---------------------------------------------------------------------------

    /**
     * Computes the greatest common divisor (GCD) of two integers.
     *
     * @tparam Mn The integer type.
     * @param _m The first integer.
     * @param _n The second integer.
     * @return The GCD of _m and _n.
     */
    template <typename Mn> constexpr auto const_gcd(const Mn& _m, const Mn& _n) -> Mn {
        if (_m == 0) {
            return _n < 0 ? -_n : _n;
        }
        return gcd_recur(_m, _n);
    }

    // ---------------------------------------------------------------------------
    // Fraction<T> — a generic rational number type
    // ---------------------------------------------------------------------------

    /// @brief A generic rational number (fraction) with signed integer arithmetic.
    ///
    /// Represents the number @f$\frac{n}{d}@f$ where the numerator @f$n@f$ and
    /// denominator @f$d@f$ are of the same signed integer type @c T.
    ///
    /// **Special values:**
    ///
    /// @dot
    ///   digraph special_values {
    ///     bgcolor="transparent";
    ///     rankdir=LR;
    ///     node [shape=box, style=filled];
    ///     zero    [label="Zero\nn = 0, d \ne 0", fillcolor="#d5f5e3"];
    ///     one     [label="One\nn = d \ne 0", fillcolor="#a9cce3"];
    ///     inf     [label="Infinity\nn = \pm 1, d = 0", fillcolor="#fadbd8"];
    ///     nan     [label="NaN\nn = 0, d = 0", fillcolor="#fcf3cf"];
    ///     rational [label="Rational\nn/d reduced", fillcolor="#d4e6f1"];
    ///     zero -> one [style=dashed, color="#888", constraint=false];
    ///     rational -> zero [label="n=0"];
    ///     rational -> one [label="n=d"];
    ///     rational -> inf [label="d=0, n\neq0"];
    ///     rational -> nan [label="n=0, d=0", color="#e74c3c"];
    ///   }
    /// @enddot
    ///
    /// **Invariants:**
    /// - The denominator is always kept positive (sign stored in numerator).
    /// - Fractions are always reduced to lowest terms after @ref from() or
    ///   @ref normalize().
    /// - Raw construction via @ref from_raw() bypasses normalization.
    ///
    /// @tparam T A signed integer type (e.g. int, int64_t, int8_t)
    template <typename T> class Fraction {
        static_assert(std::is_integral_v<T> && std::is_signed_v<T>,
                      "Fraction<T> requires a signed integer type for T");

        T numer_;
        T denom_;

        // Private: normalised constructor used by from()
        constexpr Fraction(T num, T den, bool /*tag*/) noexcept : numer_(num), denom_(den) {
            normalize();
        }

      public:
        // -----------------------------------------------------------------------
        // Constructors
        // -----------------------------------------------------------------------

        /// @brief Default constructor: zero.
        ///
        /// Constructs the fraction @f$0/1@f$.
        constexpr Fraction() noexcept : numer_(0), denom_(1) {}

        /// @brief Raw construction without normalization.
        ///
        /// Creates @f$\frac{num}{den}@f$ without normalizing. The caller
        /// must ensure the result is in a valid state. Intended for
        /// performance-critical paths where normalization is handled
        /// separately.
        ///
        /// @param[in] num Numerator
        /// @param[in] den Denominator
        /// @return A raw (possibly unnormalized) fraction
        constexpr static Fraction from_raw(T num, T den) noexcept {
            Fraction f;
            f.numer_ = num;
            f.denom_ = den;
            return f;
        }

        /// @brief Normalised construction.
        ///
        /// Constructs @f$\frac{num}{den}@f$, then normalizes: the denominator
        /// is made positive and the fraction is reduced to lowest terms.
        ///
        /// @param[in] num Numerator
        /// @param[in] den Denominator (zero yields infinity; zero/zero yields NaN)
        /// @return A normalized fraction
        constexpr static Fraction from(T num, T den) noexcept { return Fraction(num, den, true); }

        /// @brief Zero constant: @f$0/1@f$.
        constexpr static Fraction zero() noexcept { return from_raw(0, 1); }

        /// @brief One constant: @f$1/1@f$.
        constexpr static Fraction one() noexcept { return from_raw(1, 1); }

        // -----------------------------------------------------------------------
        // Accessors
        // -----------------------------------------------------------------------

        /// @brief Return the numerator @f$n@f$.
        constexpr const T& numer() const noexcept { return numer_; }

        /// @brief Return the denominator @f$d@f$ (always positive).
        constexpr const T& denom() const noexcept { return denom_; }

        // -----------------------------------------------------------------------
        // Predicates
        // -----------------------------------------------------------------------

        /// @brief Check whether the fraction equals zero.
        ///
        /// A fraction is zero when its numerator is zero and its denominator
        /// is non-zero: @f$\frac{0}{d} = 0@f$.
        constexpr bool is_zero() const noexcept { return numer_ == 0 && denom_ != 0; }

        /// @brief Check whether the fraction equals one.
        ///
        /// A fraction equals one when numerator and denominator are equal
        /// and non-zero: @f$\frac{n}{n} = 1@f$.
        constexpr bool is_one() const noexcept { return numer_ == denom_ && numer_ != 0; }

        /// @brief Check whether the fraction represents infinity.
        ///
        /// Infinity occurs when the denominator is zero and the numerator
        /// is non-zero: @f$\frac{\pm 1}{0} = \pm\infty@f$.
        constexpr bool is_infinite() const noexcept { return numer_ != 0 && denom_ == 0; }

        /// @brief Check whether the fraction is NaN (Not-a-Number).
        ///
        /// NaN occurs when both numerator and denominator are zero:
        /// @f$\frac{0}{0}@f$ is undefined.
        constexpr bool is_nan() const noexcept { return numer_ == 0 && denom_ == 0; }

        /// @brief Check whether the fraction is strictly positive.
        constexpr bool is_positive() const noexcept { return numer_ > 0; }

        /// @brief Check whether the fraction is strictly negative.
        constexpr bool is_negative() const noexcept { return numer_ < 0; }

        // -----------------------------------------------------------------------
        // Mutators
        // -----------------------------------------------------------------------

        /// @brief Set the fraction to zero: @f$0/1@f$.
        constexpr void set_zero() noexcept {
            numer_ = 0;
            denom_ = 1;
        }

        /// @brief Set the fraction to one: @f$1/1@f$.
        constexpr void set_one() noexcept {
            numer_ = 1;
            denom_ = 1;
        }

        /// @brief Set the fraction to positive infinity: @f$1/0@f$.
        constexpr void set_infinite() noexcept {
            numer_ = 1;
            denom_ = 0;
        }

        /// @brief Set the fraction to NaN: @f$0/0@f$.
        constexpr void set_nan() noexcept {
            numer_ = 0;
            denom_ = 0;
        }

        /// @brief Ensure the denominator is positive.
        ///
        /// If @f$d < 0@f$, flips both signs:
        /// @f$\frac{n}{d} = \frac{-n}{-d}@f$.
        constexpr void keep_denom_positive() noexcept {
            if (denom_ < 0) {
                numer_ = -numer_;
                denom_ = -denom_;
            }
        }

        /// @brief Reduce the fraction to lowest terms.
        ///
        /// Divides numerator and denominator by their GCD:
        /// @f[
        ///     \frac{n}{d} \to \frac{n / g}{d / g}, \qquad
        ///     g = \gcd(n, d)
        /// @f]
        ///
        /// @return The GCD @f$g@f$ that was divided out.
        constexpr T reduce() noexcept {
            T g = const_gcd(numer_, denom_);
            if (g != 0 && g != 1) {
                numer_ /= g;
                denom_ /= g;
            }
            return g;
        }

        /// @brief Normalise the fraction.
        ///
        /// First ensures the denominator is positive, then reduces to
        /// lowest terms. Equivalent to:
        /// @f[
        ///     \frac{n}{d} \to
        ///     \frac{ \operatorname{sgn}(d) \cdot n / g }
        ///          { |d| / g },
        ///     \qquad g = \gcd(n, d)
        /// @f]
        ///
        /// @return The GCD @f$g@f$ that was divided out.
        constexpr T normalize() noexcept {
            keep_denom_positive();
            return reduce();
        }

        /// @brief Swap numerator and denominator (invert the fraction).
        ///
        /// @f[
        ///     \left(\frac{n}{d}\right)^{-1} = \frac{d}{n}
        /// @f]
        ///
        /// After swapping, @ref keep_denom_positive() is called to
        /// maintain the invariant.
        constexpr void reciprocal() noexcept {
            T tmp = numer_;
            numer_ = denom_;
            denom_ = tmp;
            keep_denom_positive();
        }

        // -----------------------------------------------------------------------
        // Computed properties
        // -----------------------------------------------------------------------

        /// @brief Return the absolute value of the fraction.
        ///
        /// @f[
        ///     \left|\frac{n}{d}\right| =
        ///     \begin{cases}
        ///         \frac{n}{d}  & \text{if } n \ge 0 \\[2pt]
        ///         -\frac{n}{d} & \text{if } n < 0
        ///     \end{cases}
        /// @f]
        constexpr Fraction abs() const noexcept { return is_negative() ? -*this : *this; }

        /** @brief Return the signum (sign) of the fraction.
         *
         *  @f[
         *      \operatorname{sgn}\!\left(\frac{n}{d}\right) =
         *      \begin{cases}
         *          1   & \text{if } n > 0 \\
         *          0   & \text{if } n = 0 \\
         *          -1  & \text{if } n < 0
         *      \end{cases}
         *  @f] */
        constexpr Fraction signum() const noexcept {
            if (is_positive()) return one();
            if (is_zero()) return zero();
            return -one();
        }

        /// @brief Compute the cross product with another fraction.
        ///
        /// @f[
        ///     \operatorname{cross}\!\left(\frac{a}{b},
        ///                             \frac{c}{d}\right) = a d - b c
        /// @f]
        ///
        /// This is the determinant of the 2x2 matrix formed by the
        /// two fractions' numerators and denominators.
        ///
        /// @param[in] rhs The right-hand side fraction @f$c/d@f$
        /// @return The scalar value @f$ad - bc@f$
        constexpr T cross(const Fraction& rhs) const noexcept {
            return numer_ * rhs.denom_ - denom_ * rhs.numer_;
        }

        /// @brief Compute the multiplicative inverse.
        ///
        /// @f[
        ///     \left(\frac{n}{d}\right)^{-1} = \frac{d}{n}
        /// @f]
        ///
        /// @return The reciprocal fraction
        constexpr Fraction inv() const noexcept {
            Fraction r = *this;
            r.reciprocal();
            return r;
        }

        // -----------------------------------------------------------------------
        // Unary operators
        // -----------------------------------------------------------------------

        constexpr Fraction operator-() const noexcept { return from_raw(-numer_, denom_); }

        // -----------------------------------------------------------------------
        // Compound assignment (Fraction)
        // -----------------------------------------------------------------------

        /// @brief Add a fraction to this one.
        ///
        /// @f[
        ///     \frac{a}{b} + \frac{c}{d} = \frac{ad + bc}{bd}
        /// @f]
        ///
        /// Handles NaN/Inf propagation and uses cross-reduction for
        /// smaller intermediate values.
        ///
        /// @param[in] rhs The fraction @f$c/d@f$ to add
        /// @return Reference to @c *this
        constexpr Fraction& operator+=(const Fraction& rhs) noexcept {
            if (is_nan() || rhs.is_nan()) {
                set_nan();
                return *this;
            }
            if (is_infinite() && rhs.is_infinite()) {
                if (is_negative() != rhs.is_negative()) set_nan();
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

        /// @brief Subtract a fraction from this one.
        ///
        /// @f[
        ///     \frac{a}{b} - \frac{c}{d} = \frac{ad - bc}{bd}
        /// @f]
        ///
        /// @param[in] rhs The fraction @f$c/d@f$ to subtract
        /// @return Reference to @c *this
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

            numer_ = cross(tmp);  // a*d - b*c
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

        /// @brief Multiply this fraction by another.
        ///
        /// @f[
        ///     \frac{a}{b} \times \frac{c}{d} = \frac{ac}{bd}
        /// @f]
        ///
        /// @param[in] rhs The fraction @f$c/d@f$ to multiply by
        /// @return Reference to @c *this
        constexpr Fraction& operator*=(const Fraction& rhs) noexcept {
            // if (is_nan() || rhs.is_nan()) {
            //     set_nan();
            //     return *this;
            // }
            // if ((is_infinite() && rhs.is_zero()) || (is_zero() && rhs.is_infinite())) {
            //     set_nan();
            //     return *this;
            // }
            // if (is_zero() || rhs.is_zero()) {
            //     set_zero();
            //     return *this;
            // }
            // if (is_infinite() || rhs.is_infinite()) {
            //     bool neg = is_negative() != rhs.is_negative();
            //     numer_ = neg ? -1 : 1;
            //     denom_ = 0;
            //     return *this;
            // }

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

        /// @brief Divide this fraction by another.
        ///
        /// @f[
        ///     \frac{a}{b} \div \frac{c}{d} = \frac{ad}{bc}
        /// @f]
        ///
        /// @param[in] rhs The fraction @f$c/d@f$ to divide by
        /// @return Reference to @c *this
        constexpr Fraction& operator/=(const Fraction& rhs) noexcept {
            // if (is_nan() || rhs.is_nan()) {
            //     set_nan();
            //     return *this;
            // }
            // if (is_infinite() && rhs.is_infinite()) {
            //     set_nan();
            //     return *this;
            // }
            // if (is_zero() && rhs.is_zero()) {
            //     set_nan();
            //     return *this;
            // }
            // if (rhs.is_zero()) {
            //     numer_ = is_negative() ? -1 : 1;
            //     denom_ = 0;
            //     return *this;
            // }
            // if (is_infinite()) return *this;
            // if (rhs.is_infinite()) {
            //     set_zero();
            //     return *this;
            // }

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
            // n/d + rhs = (n + d*rhs) / d
            if (denom_ == 1) {
                numer_ += rhs;
                return *this;
            }
            numer_ += denom_ * rhs;
            reduce();
            return *this;
        }

        constexpr Fraction& operator-=(T rhs) noexcept {
            // n/d - rhs = (n - d*rhs) / d
            if (denom_ == 1) {
                numer_ -= rhs;
                return *this;
            }
            numer_ -= denom_ * rhs;
            reduce();
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

        /// @brief Equality comparison: @f$\frac{a}{b} = \frac{c}{d} \iff a = c \land b = d@f$.
        constexpr bool operator==(const Fraction& rhs) const noexcept {
            if (denom_ == rhs.denom_) return numer_ == rhs.numer_;
            // Cross-multiply with reduction
            Fraction lhs_f = from_raw(numer_, rhs.numer_);
            Fraction rhs_f = from_raw(denom_, rhs.denom_);
            lhs_f.reduce();
            rhs_f.reduce();
            return lhs_f.numer_ * rhs_f.denom_ == lhs_f.denom_ * rhs_f.numer_;
        }

        /// @brief Inequality comparison.
        constexpr bool operator!=(const Fraction& rhs) const noexcept { return !(*this == rhs); }

        /// @brief Less-than comparison.
        ///
        /// Compares two fractions by cross-multiplication:
        /// @f[
        ///     \frac{a}{b} < \frac{c}{d} \iff ad < bc
        /// @f]
        /// If both denominators are equal, the numerators are compared directly.
        constexpr bool operator<(const Fraction& rhs) const noexcept {
            if (denom_ == rhs.denom_) return numer_ < rhs.numer_;
            // Cross-multiply with reduction
            Fraction lhs_f = from_raw(numer_, rhs.numer_);
            Fraction rhs_f = from_raw(denom_, rhs.denom_);
            lhs_f.reduce();
            rhs_f.reduce();
            return lhs_f.numer_ * rhs_f.denom_ < lhs_f.denom_ * rhs_f.numer_;
        }

        constexpr bool operator<=(const Fraction& rhs) const noexcept { return !(rhs < *this); }

        constexpr bool operator>(const Fraction& rhs) const noexcept { return rhs < *this; }

        constexpr bool operator>=(const Fraction& rhs) const noexcept { return !(*this < rhs); }

        // -----------------------------------------------------------------------
        // Comparison with scalar
        // -----------------------------------------------------------------------

        constexpr bool operator==(T rhs) const noexcept { return denom_ == 1 && numer_ == rhs; }

        constexpr bool operator!=(T rhs) const noexcept { return !(*this == rhs); }

        constexpr bool operator<(T rhs) const noexcept {
            if (denom_ == 1 || rhs == 0) return numer_ < rhs;
            Fraction f = from_raw(numer_, rhs);
            f.reduce();
            return f.numer_ < f.denom_ * denom_;
        }

        constexpr bool operator<=(T rhs) const noexcept { return !(rhs < *this); }

        constexpr bool operator>(T rhs) const noexcept { return rhs < *this; }

        constexpr bool operator>=(T rhs) const noexcept { return !(*this < rhs); }

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
            if (rhs.denom_ == 1 || lhs == 0) return lhs < rhs.numer_;
            Fraction f = Fraction::from_raw(lhs, 1);
            return f < rhs;
        }

        friend constexpr bool operator<=(T lhs, const Fraction& rhs) noexcept {
            return !(rhs < lhs);
        }

        friend constexpr bool operator>(T lhs, const Fraction& rhs) noexcept { return rhs < lhs; }

        friend constexpr bool operator>=(T lhs, const Fraction& rhs) noexcept {
            return !(lhs < rhs);
        }

        // -----------------------------------------------------------------------
        // Streaming
        // -----------------------------------------------------------------------

        /// @brief Stream output.
        ///
        /// Prints the fraction as @c "n/d", or @c "inf", @c "-inf", @c "nan"
        /// for special values. Integer values (denominator = 1) print as
        /// a plain number.
        template <typename OStream> friend OStream& operator<<(OStream& os, const Fraction& f) {
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

    /// @brief Compute a triangle-related discriminant using Archimedes' formula.
    ///
    /// Given three fractional side lengths @f$a, b, c@f$, computes:
    /// @f[
    ///     \Delta = 4ab - (a + b - c)^2
    /// @f]
    ///
    /// This quantity is related to the triangle's area via Heron's formula.
    /// For a valid triangle with sides satisfying the triangle inequality,
    /// @f$\Delta > 0@f$.
    ///
    /// @dot
    ///   digraph archimedes_delta {
    ///     bgcolor="transparent";
    ///     rankdir=LR;
    ///     node [shape=box, style=filled, fillcolor="#d4e6f1"];
    ///     ab  [label="4ab", fillcolor="#a9cce3"];
    ///     sum [label="a + b - c", fillcolor="#f9e79f"];
    ///     sq  [label="(a+b-c)^2", fillcolor="#fadbd8"];
    ///     sub [label="\u0394 = 4ab - (a+b-c)^2", fillcolor="#7fb3d8"];
    ///     ab -> sub;
    ///     sum -> sq -> sub;
    ///   }
    /// @enddot
    ///
    /// @tparam T A Fraction type (must support @ref Fraction::from_raw)
    /// @param[in] side_a First side length
    /// @param[in] side_b Second side length
    /// @param[in] side_c Third side length
    /// @return The discriminant @f$\Delta@f$
    template <typename T>
    constexpr T archimedes(const T& side_a, const T& side_b, const T& side_c) {
        T temp_sum = side_a + side_b - side_c;
        return T::from_raw(4, 1) * side_a * side_b - temp_sum * temp_sum;
    }

}  // namespace fraction
