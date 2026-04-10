#pragma once

class algebra::Fraction {
    static constexpr auto serial_class = detail::SerialClass::FRACTION;

    void simplify() {
        if (denominator == 0) {
            numerator = numerator < 0 ? -1 : 1;
            return;
        }
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        if (numerator == 0) {
            denominator = 1;
        } else {
            const BigInt gcd = std::gcd(numerator, denominator);
            numerator /= gcd;
            denominator /= gcd;
        }
    }

public:
    BigInt numerator, denominator;

    Fraction(const int numerator = 0, const int denominator = 1) : Fraction(BigInt(numerator), BigInt(denominator)) {}

    Fraction(const BigInt& numerator, const BigInt& denominator) : numerator(numerator), denominator(denominator) { simplify(); }

    template <typename T>
        requires std::floating_point<T>
    Fraction(T value) : numerator(0), denominator(1) {
        for (int i = 0; i < 20 && static_cast<__int128_t>(value) != value; i++) {
            denominator *= 10;
            value *= 10;
        }
        numerator = value;
        simplify();
    }

    Fraction operator-() const { return Fraction(-numerator, denominator); }

    Fraction& operator+=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            numerator = numerator * value.denominator + value.numerator * denominator;
            denominator *= value.denominator;
            simplify();
        }
        return *this;
    }

    Fraction operator+(const Fraction& value) const { return Fraction(*this) += value; }

    Fraction& operator-=(const Fraction& value) { return *this += -value; }

    Fraction operator-(const Fraction& value) const { return *this + -value; }

    Fraction& operator*=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            numerator *= value.numerator;
            denominator *= value.denominator;
            simplify();
        }
        return *this;
    }

    Fraction operator*(const Fraction& value) const { return Fraction(*this) *= value; }

    Fraction& operator/=(const Fraction& value) {
        if (value.is_infinity()) {
            *this = value;
        } else if (!is_infinity()) {
            numerator *= value.denominator;
            denominator *= value.numerator;
            simplify();
        }
        return *this;
    }

    Fraction operator/(const Fraction& value) const { return Fraction(*this) /= value; }

    Fraction& operator^=(Fraction value) {
        if (value.is_infinity()) {
            *this = value;
            return *this;
        }
        if (is_infinity()) {
            return *this;
        }
        if (value.denominator == 1) {
            BigInt exp = value.numerator;

            if (exp < 0) {
                std::swap(numerator, denominator);
                exp = -exp;
            }
            numerator ^= exp;
            denominator ^= exp;
        } else {
            assert(numerator >= 0 || value.denominator % 2 != 0);
            BigInt root_num = numerator.nth_root(value.denominator), root_den = denominator.nth_root(value.denominator);
            assert(root_num != -1 && root_den != -1 && (root_num ^ value.denominator) == numerator && (root_den ^ value.denominator) == denominator);

            if (value.numerator < 0) {
                std::swap(root_num, root_den);
                value.numerator = -value.numerator;
            }
            numerator = root_num ^ value.numerator;
            denominator = root_den ^ value.numerator;
        }
        simplify();
        return *this;
    }

    Fraction operator^(const Fraction& value) const { return Fraction(*this) ^= value; }

    std::strong_ordering operator<=>(const Fraction& value) const { return numerator * value.denominator <=> value.numerator * denominator; }

    template <typename T>
        requires std::integral<T> || std::floating_point<T>
    std::partial_ordering operator<=>(const T& value) const {
        return static_cast<double>(*this) <=> value;
    }

    bool operator==(const Fraction& value) const = default;

    bool operator==(const long double value) const { return static_cast<double>(*this) == value; }

    template <typename T>
        requires std::integral<T> || std::floating_point<T>
    explicit operator T() const {
        return static_cast<T>(numerator) / static_cast<T>(denominator);
    }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        numerator.serialize(out);
        denominator.serialize(out);
    }

    static Fraction deserialize(std::ifstream& in) {
        detail::SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        Fraction res;
        res.numerator = BigInt::deserialize(in);
        res.denominator = BigInt::deserialize(in);
        return res;
    }

    bool is_infinity() const;

    Fraction reciprocate() const { return Fraction(denominator, numerator); }

    std::string to_latex() const;
};

namespace algebra {
    inline static auto inf = Fraction(1, 0);

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
        fraction.numerator = fraction.numerator < 0 ? -fraction.numerator : fraction.numerator;
        return fraction;
    }

    inline algebra::Fraction gcd(const algebra::Fraction& lhs, const algebra::Fraction& rhs) {
        return algebra::Fraction(gcd(lhs.numerator, rhs.numerator), lcm(lhs.denominator, rhs.denominator));
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

inline bool algebra::Fraction::is_infinity() const { return std::abs(*this) == inf; }

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

inline algebra::Fraction operator*(const int lhs, const algebra::Fraction& rhs) { return rhs * lhs; }

inline std::ostream& algebra::operator<<(std::ostream& out, const Fraction& fraction) { return out << std::to_string(fraction); }
