#pragma once

class algebra::Fraction {
    constexpr void simplify() {
        assert(denominator != 0);

        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        if (numerator == 0) {
            denominator = 1;
        } else {
            const int64_t gcd = std::gcd(numerator, denominator);
            numerator /= gcd;
            denominator /= gcd;
        }
    }

    constexpr bool is_infinity() const;

public:
    int64_t numerator, denominator;

    constexpr Fraction(const int numerator = 0, const int denominator = 1) : numerator(numerator), denominator(denominator) { simplify(); }

    constexpr Fraction(const int64_t numerator, const int64_t denominator = 1) : numerator(numerator), denominator(denominator) { simplify(); }

    constexpr Fraction(double value) : numerator(0), denominator(1) {
        for (int i = 0; i < 9 && static_cast<int64_t>(value) != value; i++) {
            denominator *= 10;
            value *= 10;
        }
        numerator = value;
        simplify();
    }

    constexpr Fraction operator-() const { return Fraction(-numerator, denominator); }

    constexpr Fraction& operator+=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            numerator = numerator * value.denominator + value.numerator * denominator;
            denominator *= value.denominator;
            simplify();
        }
        return *this;
    }

    constexpr Fraction operator+(const Fraction& value) const {
        Fraction fraction = *this;
        fraction += value;
        return fraction;
    }

    constexpr Fraction& operator-=(const Fraction& value) { return *this += -value; }

    constexpr Fraction operator-(const Fraction& value) const {
        Fraction fraction = *this;
        fraction -= value;
        return fraction;
    }

    constexpr Fraction& operator*=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            numerator *= value.numerator;
            denominator *= value.denominator;
            simplify();
        }
        return *this;
    }

    constexpr Fraction operator*(const Fraction& value) const {
        Fraction fraction = *this;
        fraction *= value;
        return fraction;
    }

    constexpr Fraction& operator/=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            numerator *= value.denominator;
            denominator *= value.numerator;
            simplify();
        }
        return *this;
    }

    constexpr Fraction operator/(const Fraction& value) const {
        Fraction fraction = *this;
        fraction /= value;
        return fraction;
    }

    constexpr Fraction& operator^=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            const double exponent = static_cast<double>(value);
            *this = Fraction(std::pow(numerator, exponent)) / Fraction(std::pow(denominator, exponent));
        }
        return *this;
    }

    constexpr Fraction operator^(const Fraction& value) const {
        Fraction fraction = *this;
        fraction ^= value;
        return fraction;
    }

    constexpr std::strong_ordering operator<=>(const Fraction& value) const {
        return static_cast<int128_t>(numerator) * value.denominator <=> static_cast<int128_t>(value.numerator) * denominator;
    }

    constexpr std::partial_ordering operator<=>(const double value) const { return static_cast<double>(*this) <=> value; }

    constexpr bool operator==(const Fraction& value) const = default;

    constexpr bool operator==(const double value) const { return static_cast<double>(*this) == value; }

    constexpr explicit operator double() const { return static_cast<double>(numerator) / denominator; }
};

inline static constexpr algebra::Fraction inf = INT64_MAX;

namespace std {
    inline algebra::Fraction abs(algebra::Fraction fraction) {
        fraction.numerator = abs(fraction.numerator);
        return fraction;
    }

    inline algebra::Fraction gcd(const algebra::Fraction& lhs, const algebra::Fraction& rhs) {
        return {gcd(lhs.numerator, rhs.numerator), lcm(lhs.denominator, rhs.denominator)};
    }

    inline algebra::Fraction& max(algebra::Fraction& lhs, algebra::Fraction& rhs) { return lhs < rhs ? rhs : lhs; }

    inline algebra::Fraction& min(algebra::Fraction& lhs, algebra::Fraction& rhs) { return lhs <= rhs ? lhs : rhs; }

    inline string to_string(const algebra::Fraction& fraction) {
        if (fraction == inf) {
            return "inf";
        }
        if (fraction == -inf) {
            return "-inf";
        }
        std::string res = std::to_string(fraction.numerator);

        if (fraction.denominator != 1) {
            res.push_back('/');
            res.append(to_string(fraction.denominator));
        }
        return res;
    }
} // namespace std

constexpr bool algebra::Fraction::is_infinity() const { return std::abs(*this) == inf; }

inline std::ostream& algebra::operator<<(std::ostream& out, const Fraction& fraction) { return out << std::to_string(fraction); }
