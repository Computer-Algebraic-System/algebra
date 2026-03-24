#pragma once

template <typename T>
class algebra::detail::AlgebraicContainer {
    void simplify() {
        assert(!denominator.terms.empty());

        if (numerator.terms.empty()) {
            denominator.terms.clear();
            denominator.terms.emplace_back(1);
            return;
        }
        Fraction coefficient_gcd = denominator.terms.front().coefficient;
        std::map<std::string, Fraction> value_gcd;

        for (const auto variable : denominator.terms.front().variables) {
            value_gcd.emplace(variable.name, variable.exponent);
        }
        for (const T& value : std::array{numerator.terms, denominator.terms} | std::views::join) {
            coefficient_gcd = std::gcd(coefficient_gcd, value.coefficient);

            for (auto& [name, exponent] : value_gcd) {
                const auto itr = std::ranges::find(value.variables, name, &Variable::Var::name);

                if (itr != value.variables.end()) {
                    exponent = std::min(exponent, itr->exponent);
                } else {
                    exponent = 0;
                }
            }
        }
        T factor = coefficient_gcd;

        for (const auto& [name, exponent] : value_gcd) {
            factor *= T(name) ^ exponent;
        }
        for (T& value : numerator.terms) {
            value /= factor;
        }
        for (T& value : denominator.terms) {
            value /= factor;
        }
    }

public:
    AlgebraicExpression<T> numerator, denominator;

    AlgebraicContainer() : numerator(), denominator(1) {}

    AlgebraicContainer(const Fraction& value) : numerator(value), denominator(1) { simplify(); }

    AlgebraicContainer(const T& value) : numerator(value), denominator(1) { simplify(); }

    AlgebraicContainer(const AlgebraicExpression<T>& numerator, const AlgebraicExpression<T>& denominator) :
        numerator(numerator), denominator(denominator) {
        simplify();
    }

    AlgebraicContainer operator-() const {
        AlgebraicContainer res = *this;
        res.numerator = -res.numerator;
        return res;
    }

    AlgebraicContainer& operator+=(const Fraction& value) { return *this += AlgebraicContainer(value); }

    AlgebraicContainer operator+(const Fraction& value) const { return *this + AlgebraicContainer(value); }

    AlgebraicContainer& operator+=(const T& value) { return *this += AlgebraicContainer(value); }

    AlgebraicContainer operator+(const T& value) const { return *this + AlgebraicContainer(value); }

    AlgebraicContainer& operator+=(const AlgebraicContainer& value) { return *this = *this + value; }

    AlgebraicContainer operator+(const AlgebraicContainer& value) const {
        return AlgebraicContainer(numerator * value.denominator + denominator * value.numerator, denominator * value.denominator);
    }

    AlgebraicContainer& operator-=(const Fraction& value) { return *this += -value; }

    AlgebraicContainer operator-(const Fraction& value) const { return *this + -value; }

    AlgebraicContainer& operator-=(const T& value) { return *this += -value; }

    AlgebraicContainer operator-(const T& value) const { return *this + -value; }

    AlgebraicContainer& operator-=(const AlgebraicContainer& value) { return *this += -value; }

    AlgebraicContainer operator-(const AlgebraicContainer& value) const { return *this + -value; }

    AlgebraicContainer& operator*=(const Fraction& value) { return *this *= AlgebraicContainer(value); }

    AlgebraicContainer operator*(const Fraction& value) const { return *this * AlgebraicContainer(value); }

    AlgebraicContainer& operator*=(const T& value) { return *this *= AlgebraicContainer(value); }

    AlgebraicContainer operator*(const T& value) const { return *this * AlgebraicContainer(value); }

    AlgebraicContainer& operator*=(const AlgebraicContainer& value) { return *this = *this * value; }

    AlgebraicContainer operator*(const AlgebraicContainer& value) const {
        return AlgebraicContainer(numerator * value.numerator, denominator * value.denominator);
    }

    AlgebraicContainer& operator/=(const Fraction& value) { return *this /= AlgebraicContainer(value); }

    AlgebraicContainer operator/(const Fraction& value) const { return *this / AlgebraicContainer(value); }

    AlgebraicContainer& operator/=(const T& value) { return *this /= AlgebraicContainer(value); }

    AlgebraicContainer operator/(const T& value) const { return *this / AlgebraicContainer(value); }

    AlgebraicContainer& operator/=(const AlgebraicContainer& value) { return *this = *this / value; }

    AlgebraicContainer operator/(const AlgebraicContainer& value) const {
        return AlgebraicContainer(numerator * value.denominator, denominator * value.numerator);
    }

    std::map<T, Fraction> roots() const;

    AlgebraicContainer substitute(const std::vector<std::pair<T, Fraction>>& values) const {
        return AlgebraicContainer(numerator.substitute(values), denominator.substitute(values));
    }

    AlgebraicContainer differentiate(const T& wrt) const {
        return AlgebraicContainer(denominator * numerator.differentiate(wrt) - numerator * denominator.differentiate(wrt), denominator * denominator);
    }

    bool is_numerator() const { return denominator.is_fraction() and static_cast<Fraction>(denominator) == 1; }

    bool is_value() const { return is_numerator() && numerator.is_value(); }

    bool is_fraction() const { return is_numerator() && numerator.is_fraction(); }

    std::string to_latex() const {
        if (numerator.terms.empty()) {
            return "0";
        }
        std::string res;

        if (!denominator.is_fraction() || static_cast<Fraction>(denominator) != 1) {
            res.append("\\frac{");
        }
        res.append(numerator.to_latex());

        if (!denominator.is_fraction() || static_cast<Fraction>(denominator) != 1) {
            res.append("}{");
        }
        if (!denominator.is_fraction() || static_cast<Fraction>(denominator) != 1) {
            res.append(denominator.to_latex()).push_back('}');
        }
        return res;
    }

    explicit operator Fraction() const {
        assert(is_fraction());
        return static_cast<Fraction>(static_cast<T>(numerator));
    }

    explicit operator T() const {
        assert(is_value());
        return numerator.terms.front();
    }
};

template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const T& lhs, const T& rhs) {
    return algebra::detail::AlgebraicContainer(lhs) + rhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const T& lhs, const algebra::Fraction& rhs) {
    return algebra::detail::AlgebraicContainer(lhs) + rhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const algebra::Fraction& lhs, const T& rhs) {
    return rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const algebra::Fraction& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs + lhs;
}

template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const T& lhs, const T& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const T& lhs, const algebra::Fraction& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const algebra::Fraction& lhs, const T& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const algebra::Fraction& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return -rhs + lhs;
}

template <typename T>
algebra::detail::AlgebraicContainer<T> operator*(const algebra::Fraction& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs * lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator*(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs * lhs;
}

namespace std {
    template <typename T>
    string to_string(const algebra::detail::AlgebraicContainer<T>& algebraic_container) {
        if (algebraic_container.numerator.terms.empty()) {
            return "0";
        }
        string res;

        if (algebraic_container.numerator.terms.size() > 1 &&
            (!algebraic_container.denominator.is_fraction() || static_cast<algebra::Fraction>(algebraic_container.denominator) != 1)) {
            res.push_back('(');
        }
        res.append(to_string(algebraic_container.numerator));

        if (algebraic_container.numerator.terms.size() > 1 &&
            (!algebraic_container.denominator.is_fraction() || static_cast<algebra::Fraction>(algebraic_container.denominator) != 1)) {
            res.push_back(')');
        }

        if (!algebraic_container.denominator.is_fraction() || static_cast<algebra::Fraction>(algebraic_container.denominator) != 1) {
            res.push_back('/');

            if (algebraic_container.denominator.terms.size() > 1) {
                res.push_back('(');
            }
            res.append(to_string(algebraic_container.denominator));

            if (algebraic_container.denominator.terms.size() > 1) {
                res.push_back(')');
            }
        }
        return res;
    }
} // namespace std

template <typename T>
std::ostream& algebra::detail::operator<<(std::ostream& out, const AlgebraicContainer<T>& algebraic_container) {
    return out << std::to_string(algebraic_container);
}
