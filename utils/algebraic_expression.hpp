#pragma once

template <typename T>
    requires std::is_same_v<T, algebra::Variable> || std::is_same_v<T, algebra::Function>
class algebra::detail::AlgebraicExpression {
public:
    std::vector<T> terms;

    constexpr AlgebraicExpression() = default;

    AlgebraicExpression(const Fraction& fraction) : AlgebraicExpression(T(fraction)) {}

    AlgebraicExpression(const T& value) : terms({value}) {}

    AlgebraicExpression operator-() const {
        AlgebraicExpression res = *this;

        for (T& value : res.terms) {
            value *= -1;
        }
        return res;
    }

    AlgebraicExpression& operator+=(const Fraction& value) { return *this += AlgebraicExpression(value); }

    AlgebraicExpression operator+(const Fraction& value) const { return *this + AlgebraicExpression(value); }

    AlgebraicExpression& operator+=(const T& value) { return *this += AlgebraicExpression(value); }

    AlgebraicExpression operator+(const T& value) const { return *this + AlgebraicExpression(value); }

    AlgebraicExpression& operator+=(const AlgebraicExpression& other) {
        for (const T& value : other.terms) {
            const auto itr = std::ranges::lower_bound(terms, value, [](const T& lhs, const T& rhs) -> bool {
                const bool lhs_const = lhs.is_fraction(), rhs_const = rhs.is_fraction();

                if (lhs_const != rhs_const) {
                    return !lhs_const;
                }
                return lhs.variables < rhs.variables;
            });

            if (itr != terms.end() && itr->variables == value.variables) {
                itr->coefficient += value.coefficient;

                if (itr->coefficient == 0) {
                    terms.erase(itr);
                }
            } else if (value.coefficient != 0) {
                terms.insert(itr, value);
            }
        }
        return *this;
    }

    AlgebraicExpression operator+(const AlgebraicExpression& value) const { return AlgebraicExpression(*this) += value; }

    AlgebraicExpression& operator-=(const Fraction& value) { return *this += -value; }

    AlgebraicExpression operator-(const Fraction& value) const { return *this + -value; }

    AlgebraicExpression& operator-=(const T& value) { return *this += -value; }

    AlgebraicExpression operator-(const T& value) const { return *this + -value; }

    AlgebraicExpression& operator-=(const AlgebraicExpression& value) { return *this += -value; }

    AlgebraicExpression operator-(const AlgebraicExpression& value) const { return *this + -value; }

    AlgebraicExpression& operator*=(const Fraction& value) { return *this *= AlgebraicExpression(value); }

    AlgebraicExpression operator*(const Fraction& value) const { return *this * AlgebraicExpression(value); }

    AlgebraicExpression& operator*=(const T& value) { return *this *= AlgebraicExpression(value); }

    AlgebraicExpression operator*(const T& value) const { return *this * AlgebraicExpression(value); }

    AlgebraicExpression& operator*=(const AlgebraicExpression& value) { return *this = *this * value; }

    AlgebraicExpression operator*(const AlgebraicExpression& value) const {
        AlgebraicExpression res;

        for (const T& lhs : terms) {
            for (const T& rhs : value.terms) {
                res += lhs * rhs;
            }
        }
        return res;
    }

    AlgebraicExpression& operator/=(const Fraction& value) { return *this /= T(value); }

    AlgebraicExpression operator/(const Fraction& value) const { return *this / T(value); }

    AlgebraicExpression& operator/=(const T& value) {
        for (T& element : terms) {
            element /= value;
        }
        return *this;
    }

    AlgebraicExpression operator/(const T& value) const { return AlgebraicExpression(*this) /= value; }

    std::map<T, Fraction> roots() const;

    AlgebraicExpression substitute(const std::vector<std::pair<T, Fraction>>& values) const {
        AlgebraicExpression res;

        for (const T& value : terms) {
            res += value.substitute(values);
        }
        return res;
    }

    AlgebraicExpression differentiate(const T& wrt) const {
        AlgebraicExpression res;

        for (const T& var : terms) {
            res += var.differentiate(wrt);
        }
        return res;
    }

    bool is_value() const { return terms.size() == 1; }

    bool is_fraction() const { return terms.empty() || is_value() && terms.front().is_fraction(); }

    std::string to_latex() const {
        std::string res;

        if (!terms.empty()) {
            res.append(terms.front().to_latex());

            for (const T& value : terms | std::views::drop(1)) {
                res.append(value.coefficient < 0 ? " - " : " + ").append(std::abs(value).to_latex());
            }
            return res;
        }
        return "0";
    }

    explicit operator Fraction() const {
        assert(is_fraction());

        if (terms.empty()) {
            return 0;
        }
        return static_cast<Fraction>(static_cast<T>(*this));
    }

    explicit operator T() const {
        assert(is_value());
        return terms.front();
    }
};

template <typename T>
algebra::detail::AlgebraicExpression<T> operator*(const algebra::Fraction& lhs, const algebra::detail::AlgebraicExpression<T>& rhs) {
    return rhs * lhs;
}

namespace std {
    template <typename T>
    string to_string(const algebra::detail::AlgebraicExpression<T>& algebraic_expression) {
        string res;

        if (!algebraic_expression.terms.empty()) {
            res.append(to_string(algebraic_expression.terms.front()));

            for (const T& value : algebraic_expression.terms | std::views::drop(1)) {
                res.append(value.coefficient < 0 ? " - " : " + ").append(to_string(abs(value)));
            }
            return res;
        }
        return "0";
    }
} // namespace std

template <typename T>
std::ostream& algebra::detail::operator<<(std::ostream& out, const AlgebraicExpression<T>& value) {
    return out << std::to_string(value);
}
