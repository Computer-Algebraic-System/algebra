#pragma once

template <typename T>
class algebra::AlgebraicContainer {
public:
    std::vector<T> expression;

    constexpr AlgebraicContainer() = default;

    AlgebraicContainer(const Fraction& fraction) : AlgebraicContainer(T(fraction)) {}

    AlgebraicContainer(const T& value) : expression({value}) {}

    AlgebraicContainer operator-() const {
        AlgebraicContainer res = *this;

        for (T& value : res.expression) {
            value *= -1;
        }
        return res;
    }

    AlgebraicContainer& operator+=(const Fraction& value) { return *this += AlgebraicContainer(value); }

    AlgebraicContainer operator+(const Fraction& value) const { return *this + AlgebraicContainer(value); }

    AlgebraicContainer& operator+=(const T& value) { return *this += AlgebraicContainer(value); }

    AlgebraicContainer operator+(const T& value) const { return *this + AlgebraicContainer(value); }

    AlgebraicContainer& operator+=(const AlgebraicContainer& other) {
        for (const T& value : other.expression) {
            const auto itr = std::ranges::lower_bound(expression, value, [](const T& lhs, const T& rhs) -> bool {
                const bool lhs_const = lhs.variables.empty(), rhs_const = rhs.variables.empty();

                if (lhs_const != rhs_const) {
                    return !lhs_const;
                }
                return lhs.variables < rhs.variables;
            });

            if (itr != expression.end() && itr->variables == value.variables) {
                itr->coefficient += value.coefficient;

                if (itr->coefficient == 0) {
                    expression.erase(itr);
                }
            } else if (value.coefficient != 0) {
                expression.insert(itr, value);
            }
        }
        return *this;
    }

    AlgebraicContainer operator+(const AlgebraicContainer& value) const { return AlgebraicContainer(*this) += value; }

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
        AlgebraicContainer res;

        for (const T& lhs : expression) {
            for (const T& rhs : value.expression) {
                res += lhs * rhs;
            }
        }
        return res;
    }

    AlgebraicContainer& operator/=(const Fraction& value) { return *this /= T(value); }

    AlgebraicContainer operator/(const Fraction& value) const { return *this / T(value); }

    AlgebraicContainer& operator/=(const T& value) {
        for (T& element : expression) {
            element /= value;
        }
        return *this;
    }

    AlgebraicContainer operator/(const T& value) const { return AlgebraicContainer(*this) /= value; }

    std::map<T, Fraction> roots() const;

    AlgebraicContainer substitute(const std::vector<std::pair<T, Fraction>>& values) const {
        AlgebraicContainer res;

        for (const T& value : expression) {
            res += value.substitute(values);
        }
        return res;
    }

    AlgebraicContainer differentiate(const T& wrt) const {
        AlgebraicContainer res;

        for (const T& var : expression) {
            res += var.differentiate(wrt);
        }
        return res;
    }

    bool is_value() const { return expression.size() == 1; }

    bool is_fraction() const { return expression.empty() || is_value() && expression.front().is_fraction(); }

    std::string to_latex() const {
        std::string res;

        if (!expression.empty()) {
            res.append(expression.front().to_latex());

            for (const T& value : expression | std::views::drop(1)) {
                res.append(value.coefficient < 0 ? " - " : " + ").append(std::abs(value).to_latex());
            }
            return res;
        }
        return "0";
    }

    explicit operator Fraction() const {
        assert(is_fraction());

        if (expression.empty()) {
            return 0;
        }
        return static_cast<Fraction>(static_cast<T>(*this));
    }

    explicit operator T() const {
        assert(is_value());
        return expression.front();
    }
};

template <typename T>
algebra::AlgebraicContainer<T> operator+(const T& lhs, const T& rhs) {
    return algebra::AlgebraicContainer(lhs) + rhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator+(const T& lhs, const algebra::Fraction& rhs) {
    return algebra::AlgebraicContainer(lhs) + rhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator+(const algebra::Fraction& lhs, const T& rhs) {
    return rhs + lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator-(const T& lhs, const T& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator-(const T& lhs, const algebra::Fraction& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator-(const algebra::Fraction& lhs, const T& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator+(const algebra::Fraction& lhs, const algebra::AlgebraicContainer<T>& rhs) {
    return rhs + lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator-(const T& lhs, const algebra::AlgebraicContainer<T>& rhs) {
    return -rhs + lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator*(const algebra::Fraction& lhs, const algebra::AlgebraicContainer<T>& rhs) {
    return rhs * lhs;
}
template <typename T>
algebra::AlgebraicContainer<T> operator*(const T& lhs, const algebra::AlgebraicContainer<T>& rhs) {
    return rhs * lhs;
}

namespace std {
    template <typename T>
    string to_string(const algebra::AlgebraicContainer<T>& algebraic_container) {
        string res;

        if (!algebraic_container.expression.empty()) {
            res.append(to_string(algebraic_container.expression.front()));

            for (const T& value : algebraic_container.expression | std::views::drop(1)) {
                res.append(value.coefficient < 0 ? " - " : " + ").append(to_string(abs(value)));
            }
            return res;
        }
        return "0";
    }
} // namespace std

template <typename T>
std::ostream& algebra::operator<<(std::ostream& out, const AlgebraicContainer<T>& value) {
    return out << std::to_string(value);
}
