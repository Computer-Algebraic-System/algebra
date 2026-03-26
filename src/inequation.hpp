#pragma once

class algebra::Inequation {
public:
    RelationalOperator opr;
    Polynomial lhs, rhs;

    Inequation() = default;

    Inequation(const Polynomial& lhs, const RelationalOperator opr, const Polynomial& rhs) : opr(opr), lhs(lhs), rhs(rhs) {
        if (!rhs.is_fraction() || !static_cast<Fraction>(rhs) == 0) {
            const Polynomial temp = lhs / rhs;
            this->lhs = temp.numerator;
            this->rhs = temp.denominator;
        }
    }

    Inequation& operator+=(const Fraction& value) { return *this += Polynomial(value); }

    Inequation operator+(const Fraction& value) const { return *this + Polynomial(value); }

    Inequation& operator+=(const Variable& value) { return *this += Polynomial(value); }

    Inequation operator+(const Variable& value) const { return *this + Polynomial(value); }

    Inequation& operator+=(const Polynomial& value) { return *this = *this + value; }

    Inequation operator+(const Polynomial& value) const { return Inequation(lhs + value, opr, rhs + value); }

    Inequation& operator-=(const Fraction& value) { return *this += -value; }

    Inequation operator-(const Fraction& value) const { return *this + -value; }

    Inequation& operator-=(const Variable& value) { return *this += -value; }

    Inequation operator-(const Variable& value) const { return *this + -value; }

    Inequation& operator-=(const Polynomial& value) { return *this += -value; }

    Inequation operator-(const Polynomial& value) const { return *this + -value; }

    Inequation& operator*=(const Fraction& value) { return *this *= Polynomial(value); }

    Inequation operator*(const Fraction& value) const { return *this * Polynomial(value); }

    Inequation& operator*=(const Variable& value) { return *this *= Polynomial(value); }

    Inequation operator*(const Variable& value) const { return *this * Polynomial(value); }

    Inequation& operator*=(const Polynomial& value) { return *this = *this * value; }

    Inequation operator*(const Polynomial& value) const { return Inequation(lhs * value, opr, rhs * value); }

    Inequation& operator/=(const Fraction& value) { return *this /= Polynomial(value); }

    Inequation operator/(const Fraction& value) const { return *this / Polynomial(value); }

    Inequation& operator/=(const Variable& value) { return *this /= Polynomial(value); }

    Inequation operator/(const Variable& value) const { return *this / Polynomial(value); }

    Inequation& operator/=(const Polynomial& value) { return *this = *this / value; }

    Inequation operator/(const Polynomial& value) const { return Inequation(lhs / value, opr, rhs / value); }

    Inequation invert() const {
        Inequation res = *this;
        res.lhs *= -1;
        res.opr = detail::invert_relational_operator(res.opr);
        res.rhs *= -1;
        return res;
    }

    Inequation swap() const {
        Inequation res = *this;
        std::swap(res.lhs, res.rhs);
        res.opr = detail::invert_relational_operator(res.opr);
        return res;
    }

    Inequation substitute(const std::vector<std::pair<Variable, Fraction>>& values) const {
        Inequation res = *this;
        res.lhs = res.lhs.substitute(values);
        res.rhs = res.rhs.substitute(values);
        return res;
    }

    Inequation solve_for(const Variable& variable) const {
        assert(lhs.denominator.is_fraction() && rhs.denominator.is_fraction());
        Inequation res;
        res.opr = opr;

        for (const Variable& var : lhs.numerator.terms) {
            if (var.basis() == variable) {
                res.lhs += var;
            } else {
                res.rhs += -var;
            }
        }
        for (const Variable& var : rhs.numerator.terms) {
            if (var.basis() == variable) {
                res.lhs += -var;
            } else {
                res.rhs += var;
            }
        }
        if (res.lhs.is_value()) {
            res /= static_cast<Variable>(res.lhs).coefficient;
        }
        return res;
    }

    Inequation differentiate(const Variable& wrt) const { return Inequation(lhs.differentiate(wrt), opr, rhs.differentiate(wrt)); }

    std::string to_latex() const { return lhs.to_latex().append(" ").append(detail::to_latex(opr)).append(" ").append(rhs.to_latex()); }

    bool is_bool() const { return lhs.is_fraction() && rhs.is_fraction(); }

    explicit operator bool() const { return detail::evaluate_relational_operator(static_cast<Fraction>(lhs), opr, static_cast<Fraction>(rhs)); }

    explicit operator Equation() const;
};

class algebra::Equation : public Inequation {
public:
    Equation() = default;

    Equation(const Polynomial& lhs, const Polynomial& rhs) : Inequation(lhs, RelationalOperator::EQ, rhs) {}

    Equation swap() const {
        Equation res = *this;
        std::swap(res.lhs, res.rhs);
        return res;
    }
};

inline algebra::Inequation::operator Equation() const { return Equation(lhs, rhs); }

inline algebra::Inequation operator<(const algebra::Polynomial& lhs, const algebra::Polynomial& rhs) {
    return algebra::Inequation(lhs, algebra::RelationalOperator::LT, rhs);
}

inline algebra::Inequation operator<=(const algebra::Polynomial& lhs, const algebra::Polynomial& rhs) {
    return algebra::Inequation(lhs, algebra::RelationalOperator::LE, rhs);
}

inline algebra::Inequation operator>(const algebra::Polynomial& lhs, const algebra::Polynomial& rhs) {
    return algebra::Inequation(lhs, algebra::RelationalOperator::GT, rhs);
}

inline algebra::Inequation operator>=(const algebra::Polynomial& lhs, const algebra::Polynomial& rhs) {
    return algebra::Inequation(lhs, algebra::RelationalOperator::GE, rhs);
}

inline algebra::Equation operator==(const algebra::Polynomial& lhs, const algebra::Polynomial& rhs) { return algebra::Equation(lhs, rhs); }

inline algebra::Inequation operator<(const algebra::Variable& lhs, const algebra::Fraction& rhs) {
    return algebra::Polynomial(lhs) < algebra::Polynomial(rhs);
}

inline algebra::Inequation operator<=(const algebra::Variable& lhs, const algebra::Fraction& rhs) {
    return algebra::Polynomial(lhs) <= algebra::Polynomial(rhs);
}

inline algebra::Inequation operator>(const algebra::Variable& lhs, const algebra::Fraction& rhs) {
    return algebra::Polynomial(lhs) > algebra::Polynomial(rhs);
}

inline algebra::Inequation operator>=(const algebra::Variable& lhs, const algebra::Fraction& rhs) {
    return algebra::Polynomial(lhs) >= algebra::Polynomial(rhs);
}

inline algebra::Equation operator==(const algebra::Variable& lhs, const algebra::Fraction& rhs) {
    return algebra::Polynomial(lhs) == algebra::Polynomial(rhs);
}

inline algebra::Inequation operator<(const algebra::Fraction& lhs, const algebra::Variable& rhs) { return (rhs > lhs).swap(); }

inline algebra::Inequation operator<=(const algebra::Fraction& lhs, const algebra::Variable& rhs) { return (rhs >= lhs).swap(); }

inline algebra::Inequation operator>(const algebra::Fraction& lhs, const algebra::Variable& rhs) { return (rhs < lhs).swap(); }

inline algebra::Inequation operator>=(const algebra::Fraction& lhs, const algebra::Variable& rhs) { return (rhs <= lhs).swap(); }

inline algebra::Equation operator==(const algebra::Fraction& lhs, const algebra::Variable& rhs) { return (rhs == lhs).swap(); }


inline algebra::Inequation operator<(const algebra::Variable& lhs, const algebra::Variable& rhs) {
    return algebra::Polynomial(lhs) < algebra::Polynomial(rhs);
}

inline algebra::Inequation operator<=(const algebra::Variable& lhs, const algebra::Variable& rhs) {
    return algebra::Polynomial(lhs) <= algebra::Polynomial(rhs);
}

inline algebra::Inequation operator>(const algebra::Variable& lhs, const algebra::Variable& rhs) {
    return algebra::Polynomial(lhs) > algebra::Polynomial(rhs);
}

inline algebra::Inequation operator>=(const algebra::Variable& lhs, const algebra::Variable& rhs) {
    return algebra::Polynomial(lhs) >= algebra::Polynomial(rhs);
}

inline algebra::Equation operator==(const algebra::Variable& lhs, const algebra::Variable& rhs) {
    return algebra::Polynomial(lhs) == algebra::Polynomial(rhs);
}

inline algebra::Inequation operator<(const algebra::Polynomial& lhs, const algebra::Fraction& rhs) { return lhs < algebra::Polynomial(rhs); }

inline algebra::Inequation operator<=(const algebra::Polynomial& lhs, const algebra::Fraction& rhs) { return lhs <= algebra::Polynomial(rhs); }

inline algebra::Inequation operator>(const algebra::Polynomial& lhs, const algebra::Fraction& rhs) { return lhs > algebra::Polynomial(rhs); }

inline algebra::Inequation operator>=(const algebra::Polynomial& lhs, const algebra::Fraction& rhs) { return lhs >= algebra::Polynomial(rhs); }

inline algebra::Equation operator==(const algebra::Polynomial& lhs, const algebra::Fraction& rhs) { return lhs == algebra::Polynomial(rhs); }

inline algebra::Inequation operator<(const algebra::Fraction& lhs, const algebra::Polynomial& rhs) { return (rhs > lhs).swap(); }

inline algebra::Inequation operator<=(const algebra::Fraction& lhs, const algebra::Polynomial& rhs) { return (rhs >= lhs).swap(); }

inline algebra::Inequation operator>(const algebra::Fraction& lhs, const algebra::Polynomial& rhs) { return (rhs < lhs).swap(); }

inline algebra::Inequation operator>=(const algebra::Fraction& lhs, const algebra::Polynomial& rhs) { return (rhs <= lhs).swap(); }

inline algebra::Equation operator==(const algebra::Fraction& lhs, const algebra::Polynomial& rhs) { return (rhs == lhs).swap(); }

inline algebra::Inequation operator<(const algebra::Polynomial& lhs, const algebra::Variable& rhs) { return lhs < algebra::Polynomial(rhs); }

inline algebra::Inequation operator<=(const algebra::Polynomial& lhs, const algebra::Variable& rhs) { return lhs <= algebra::Polynomial(rhs); }

inline algebra::Inequation operator>(const algebra::Polynomial& lhs, const algebra::Variable& rhs) { return lhs > algebra::Polynomial(rhs); }

inline algebra::Inequation operator>=(const algebra::Polynomial& lhs, const algebra::Variable& rhs) { return lhs >= algebra::Polynomial(rhs); }

inline algebra::Equation operator==(const algebra::Polynomial& lhs, const algebra::Variable& rhs) { return lhs == algebra::Polynomial(rhs); }

inline algebra::Inequation operator<(const algebra::Variable& lhs, const algebra::Polynomial& rhs) { return (rhs > lhs).swap(); }

inline algebra::Inequation operator<=(const algebra::Variable& lhs, const algebra::Polynomial& rhs) { return (rhs >= lhs).swap(); }

inline algebra::Inequation operator>(const algebra::Variable& lhs, const algebra::Polynomial& rhs) { return (rhs < lhs).swap(); }

inline algebra::Inequation operator>=(const algebra::Variable& lhs, const algebra::Polynomial& rhs) { return (rhs <= lhs).swap(); }

inline algebra::Equation operator==(const algebra::Variable& lhs, const algebra::Polynomial& rhs) { return (rhs == lhs).swap(); }

namespace std {
    inline string to_string(const algebra::Inequation& inequation) {
        return to_string(inequation.lhs).append(" ").append(to_string(inequation.opr)).append(" ").append(to_string(inequation.rhs));
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Inequation& inequation) { return out << std::to_string(inequation); }
