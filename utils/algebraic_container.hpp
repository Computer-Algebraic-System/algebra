#pragma once

template <typename T>
    requires std::is_same_v<T, algebra::Variable> || std::is_same_v<T, algebra::Function>
class algebra::detail::AlgebraicContainer {
    static constexpr SerialClass serial_class = std::is_same_v<T, Variable> ? SerialClass::RATIONAL_POLYNOMIAL : SerialClass::RATIONAL_EXPRESSION;

    void simplify() {
        assert(!denominator.terms.empty());

        if (numerator.terms.empty()) {
            denominator.terms.clear();
            denominator.terms.emplace_back(1);
            return;
        }
        std::map<std::string, double> value_gcd;
        T factor(1);

        for (const T& value : numerator.terms) {
            for (const auto& [name, exponent] : value.variables) {
                if (exponent < 0) {
                    factor *= T(name) ^ -exponent;
                }
            }
        }
        numerator *= factor;
        denominator *= factor;
        factor = 1;

        for (const auto& [name, exponent] : denominator.terms.front().variables) {
            value_gcd.emplace(name, exponent);
        }

        for (const T& value : std::array{numerator.terms, denominator.terms} | std::views::join) {
            for (auto& [name, exponent] : value_gcd) {
                const auto itr = std::ranges::find(value.variables, name, &Variable::Var::name);

                if (itr != value.variables.end()) {
                    exponent = std::min(exponent, itr->exponent);
                } else {
                    exponent = 0;
                }
            }
        }
        for (const auto& [name, exponent] : value_gcd) {
            if (exponent != 0) {
                factor *= T(name) ^ exponent;
            }
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

    AlgebraicContainer(const double value) : numerator(value), denominator(1) {
        if (value != inf) {
            simplify();
        }
    }

    AlgebraicContainer(const T& value) : numerator(value), denominator(1) { simplify(); }

    AlgebraicContainer(const AlgebraicExpression<T>& numerator, const AlgebraicExpression<T>& denominator = {1}) :
        numerator(numerator), denominator(denominator) {
        simplify();
    }

    AlgebraicContainer operator-() const {
        AlgebraicContainer res = *this;
        res.numerator = -res.numerator;
        return res;
    }

    AlgebraicContainer& operator+=(const double value) { return *this += AlgebraicContainer(value); }

    AlgebraicContainer operator+(const double value) const { return *this + AlgebraicContainer(value); }

    AlgebraicContainer& operator+=(const T& value) { return *this += AlgebraicContainer(value); }

    AlgebraicContainer operator+(const T& value) const { return *this + AlgebraicContainer(value); }

    AlgebraicContainer& operator+=(const AlgebraicContainer& value) { return *this = *this + value; }

    AlgebraicContainer operator+(const AlgebraicContainer& value) const {
        return AlgebraicContainer(numerator * value.denominator + denominator * value.numerator, denominator * value.denominator);
    }

    AlgebraicContainer& operator-=(const double value) { return *this += -value; }

    AlgebraicContainer operator-(const double value) const { return *this + -value; }

    AlgebraicContainer& operator-=(const T& value) { return *this += -value; }

    AlgebraicContainer operator-(const T& value) const { return *this + -value; }

    AlgebraicContainer& operator-=(const AlgebraicContainer& value) { return *this += -value; }

    AlgebraicContainer operator-(const AlgebraicContainer& value) const { return *this + -value; }

    AlgebraicContainer& operator*=(const double value) { return *this *= AlgebraicContainer(value); }

    AlgebraicContainer operator*(const double value) const { return *this * AlgebraicContainer(value); }

    AlgebraicContainer& operator*=(const T& value) { return *this *= AlgebraicContainer(value); }

    AlgebraicContainer operator*(const T& value) const { return *this * AlgebraicContainer(value); }

    AlgebraicContainer& operator*=(const AlgebraicContainer& value) { return *this = *this * value; }

    AlgebraicContainer operator*(const AlgebraicContainer& value) const {
        return AlgebraicContainer(numerator * value.numerator, denominator * value.denominator);
    }

    AlgebraicContainer& operator/=(const double value) { return *this /= AlgebraicContainer(value); }

    AlgebraicContainer operator/(const double value) const { return *this / AlgebraicContainer(value); }

    AlgebraicContainer& operator/=(const T& value) { return *this /= AlgebraicContainer(value); }

    AlgebraicContainer operator/(const T& value) const { return *this / AlgebraicContainer(value); }

    AlgebraicContainer& operator/=(const AlgebraicContainer& value) { return *this = *this / value; }

    AlgebraicContainer operator/(const AlgebraicContainer& value) const {
        return AlgebraicContainer(numerator * value.denominator, denominator * value.numerator);
    }

    AlgebraicContainer substitute(const std::map<T, double>& values, const bool origin = true) const {
        AlgebraicContainer res(numerator.substitute(values, false), denominator.substitute(values, false));

        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_substitute(*this, values, res);
        }
        return res;
    }

    AlgebraicContainer differentiate(const T& wrt, const bool origin = true) const {
        AlgebraicContainer res(denominator * numerator.differentiate(wrt, false) - numerator * denominator.differentiate(wrt, false),
                               denominator * denominator);

        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_differentiate(*this, wrt, res);
        }
        return res;
    }

    AlgebraicContainer integrate(const Variable& wrt, const double a, const double b, const bool origin = true) const {
        assert(wrt.variables.size() == 1);
        static auto tol = std::pow(10, -9);
        int n = 2;
        AlgebraicContainer current, prev;
        std::map<Variable, double> substituent{{wrt, a}}, sub;

        for (const Variable& variable : std::array{numerator, denominator} | std::views::join) {
            for (const auto& [name, exponent] : variable.variables) {
                sub.emplace(name, 1);
            }
        }
        do {
            const double h = (b - a) / n;
            prev = current;
            substituent.begin()->second = a;
            current = substitute(substituent, false);

            for (int i = 1; i < n; i++) {
                substituent.begin()->second = a + i * h;
                current += (i % 2 == 0 ? 2 : 4) * substitute(substituent, false);
            }
            substituent.begin()->second = b;
            current += substitute(substituent, false);
            current *= h / 3;
            n <<= 1;
        } while (std::abs(static_cast<double>(current.substitute(sub, false)) - static_cast<double>(prev.substitute(sub, false))) > tol);
        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_integrate(*this, a, b, wrt, current);
        }
        return current;
    }

    bool is_numerator() const { return denominator.is_fraction(); }

    bool is_value() const { return is_numerator() && numerator.is_value(); }

    bool is_fraction() const { return is_numerator() && numerator.is_fraction(); }

    std::string to_latex() const {
        if (numerator.terms.empty()) {
            return "0";
        }
        std::string res;

        if (!denominator.is_fraction() || static_cast<double>(denominator) != 1) {
            res.append("\\dfrac{");
        }
        res.append(numerator.to_latex());

        if (!denominator.is_fraction() || static_cast<double>(denominator) != 1) {
            res.append("}{");
        }
        if (!denominator.is_fraction() || static_cast<double>(denominator) != 1) {
            res.append(denominator.to_latex()).push_back('}');
        }
        return res;
    }

    std::string to_html() const {
        if (numerator.terms.empty()) {
            return "<mn>0</mn>";
        }
        std::string res;

        if (!denominator.is_fraction() || static_cast<double>(denominator) != 1) {
            res.append("<mfrac><mrow>");
        }
        res.append(numerator.to_html());

        if (!denominator.is_fraction() || static_cast<double>(denominator) != 1) {
            res.append("</mrow><mrow>");
        }
        if (!denominator.is_fraction() || static_cast<double>(denominator) != 1) {
            res.append(denominator.to_html()).append("</mrow></mfrac>");
        }
        return res;
    }

    explicit operator double() const {
        assert(is_fraction());

        if (numerator.terms.empty()) {
            return 0;
        }
        return static_cast<double>(static_cast<T>(numerator)) / static_cast<double>(static_cast<T>(denominator));
    }

    explicit operator T() const {
        assert(is_value());
        return numerator.terms.front() / static_cast<double>(static_cast<T>(denominator));
    }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        numerator.serialize(out);
        denominator.serialize(out);
    }

    static AlgebraicContainer deserialize(std::ifstream& in) {
        SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        AlgebraicContainer res;
        res.numerator = AlgebraicExpression<T>::deserialize(in);
        res.denominator = AlgebraicExpression<T>::deserialize(in);
        return res;
    }
};

template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const double lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator+(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs + lhs;
}

template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const double lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator-(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return -rhs + lhs;
}

template <typename T>
algebra::detail::AlgebraicContainer<T> operator*(const double lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs * lhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator*(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return rhs * lhs;
}

template <typename T>
algebra::detail::AlgebraicContainer<T> operator/(const double lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return algebra::detail::AlgebraicContainer<T>(lhs) / rhs;
}
template <typename T>
algebra::detail::AlgebraicContainer<T> operator/(const T& lhs, const algebra::detail::AlgebraicContainer<T>& rhs) {
    return algebra::detail::AlgebraicContainer<T>(lhs) / rhs;
}

template <typename T>
algebra::detail::AlgebraicContainer<T> operator/(const algebra::detail::AlgebraicExpression<T>& lhs,
                                                 const algebra::detail::AlgebraicExpression<T>& rhs) {
    return algebra::detail::AlgebraicContainer(lhs, rhs);
}

namespace std {
    template <typename T>
    string to_string(const algebra::detail::AlgebraicContainer<T>& algebraic_container) {
        if (algebraic_container.numerator.terms.empty()) {
            return "0";
        }
        string res;

        if (algebraic_container.numerator.terms.size() > 1 &&
            (!algebraic_container.denominator.is_fraction() || static_cast<double>(algebraic_container.denominator) != 1)) {
            res.push_back('(');
        }
        res.append(to_string(algebraic_container.numerator));

        if (algebraic_container.numerator.terms.size() > 1 &&
            (!algebraic_container.denominator.is_fraction() || static_cast<double>(algebraic_container.denominator) != 1)) {
            res.push_back(')');
        }

        if (!algebraic_container.denominator.is_fraction() || static_cast<double>(algebraic_container.denominator) != 1) {
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
