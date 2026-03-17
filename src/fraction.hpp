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

    constexpr Fraction operator-() const { return {-numerator, denominator}; }

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

    constexpr Fraction operator+(const Fraction& value) const { return Fraction(*this) += value; }

    constexpr Fraction& operator-=(const Fraction& value) { return *this += -value; }

    constexpr Fraction operator-(const Fraction& value) const { return *this + -value; }

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

    constexpr Fraction operator*(const Fraction& value) const { return Fraction(*this) *= value; }

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

    constexpr Fraction operator/(const Fraction& value) const { return Fraction(*this) /= value; }

    constexpr Fraction& operator^=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            const double exponent = static_cast<double>(value);
            *this = Fraction(std::pow(numerator, exponent)) / Fraction(std::pow(denominator, exponent));
        }
        return *this;
    }

    constexpr Fraction operator^(const Fraction& value) const { return Fraction(*this) ^= value; }

    constexpr std::strong_ordering operator<=>(const Fraction& value) const {
        return static_cast<int128_t>(numerator) * value.denominator <=> static_cast<int128_t>(value.numerator) * denominator;
    }

    constexpr std::partial_ordering operator<=>(const double value) const { return static_cast<double>(*this) <=> value; }

    constexpr bool operator==(const Fraction& value) const = default;

    constexpr bool operator==(const double value) const { return static_cast<double>(*this) == value; }

    constexpr explicit operator double() const { return static_cast<double>(numerator) / denominator; }

    constexpr explicit operator int64_t() const { return numerator / denominator; }

    constexpr bool is_infinity() const;

    Fraction reciprocate() const { return {denominator, numerator}; }

    std::string to_latex() const;
};

namespace algebra {
    inline static constexpr Fraction inf = INT64_MAX;

    namespace detail {
        inline bool evaluate_relational_operator(const Fraction& lhs, const RelationalOperator opr, const Fraction& rhs) {
            switch (opr) {
            case RelationalOperator::LT:
                return lhs < rhs;

            case RelationalOperator::LE:
                return lhs <= rhs;

            case RelationalOperator::GT:
                return lhs > rhs;

            case RelationalOperator::GE:
                return lhs >= rhs;

            default:
                return lhs == rhs;
            }
        }
    } // namespace detail
} // namespace algebra

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
        string res;

        if (fraction.numerator < 0) {
            res.push_back('-');
        }
        if (fraction.is_infinity()) {
            return res.append("inf");
        }
        res.append(to_string(abs(fraction.numerator)));

        if (fraction.denominator != 1) {
            res.append("/").append(to_string(fraction.denominator));
        }
        return res;
    }
} // namespace std

constexpr bool algebra::Fraction::is_infinity() const { return std::abs(*this) == inf; }

inline std::string algebra::Fraction::to_latex() const {
    std::string res;

    if (numerator < 0) {
        res.push_back('-');
    }
    if (is_infinity()) {
        return res.append("\\infty");
    }
    if (denominator != 1) {
        res.append("\\frac{");
    }
    res.append(std::to_string(std::abs(numerator)));

    if (denominator != 1) {
        res.append("}{").append(std::to_string(denominator)).append("}");
    }
    return res;
}

inline std::ostream& algebra::operator<<(std::ostream& out, const Fraction& fraction) { return out << std::to_string(fraction); }
