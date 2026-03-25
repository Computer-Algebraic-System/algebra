#pragma once

class algebra::Interval {
public:
    Polynomial lhs, mid, rhs;
    RelationalOperator opr1, opr2;

    Interval() = default;

    Interval(const Polynomial& lhs, const RelationalOperator opr1, const Polynomial& mid, const RelationalOperator opr2, const Polynomial& rhs) :
        lhs(lhs), mid(mid), rhs(rhs), opr1(opr1), opr2(opr2) {}

    Interval(const Inequation& lhs, const RelationalOperator opr, const Polynomial& rhs) :
        lhs(lhs.lhs), mid(lhs.rhs), rhs(rhs), opr1(lhs.opr), opr2(opr) {}

    Interval& operator+=(const Fraction& value) { return *this += Polynomial(value); }

    Interval operator+(const Fraction& value) const { return *this + Polynomial(value); }

    Interval& operator+=(const Variable& value) { return *this += Polynomial(value); }

    Interval operator+(const Variable& value) const { return *this + Polynomial(value); }

    Interval& operator+=(const Polynomial& value) { return *this = *this + value; }

    Interval operator+(const Polynomial& value) const { return Interval(lhs + value, opr1, mid + value, opr2, rhs + value); }

    Interval& operator-=(const Fraction& value) { return *this += -value; }

    Interval operator-(const Fraction& value) const { return *this + -value; }

    Interval& operator-=(const Variable& value) { return *this += -value; }

    Interval operator-(const Variable& value) const { return *this + -value; }

    Interval& operator-=(const Polynomial& value) { return *this += -value; }

    Interval operator-(const Polynomial& value) const { return *this + -value; }

    Interval& operator*=(const Fraction& value) { return *this *= Polynomial(value); }

    Interval operator*(const Fraction& value) const { return *this * Polynomial(value); }

    Interval& operator*=(const Variable& value) { return *this *= Polynomial(value); }

    Interval operator*(const Variable& value) const { return *this * Polynomial(value); }

    Interval& operator*=(const Polynomial& value) { return *this = *this * value; }

    Interval operator*(const Polynomial& value) const { return Interval(lhs * value, opr1, mid * value, opr2, rhs * value); }

    Interval& operator/=(const Fraction& value) { return *this /= Polynomial(value); }

    Interval operator/(const Fraction& value) const { return *this / Polynomial(value); }

    Interval& operator/=(const Variable& value) { return *this /= Polynomial(value); }

    Interval operator/(const Variable& value) const { return *this / Polynomial(value); }

    Interval& operator/=(const Polynomial& value) { return *this = *this / value; }

    Interval operator/(const Polynomial& value) const { return Interval(lhs / value, opr1, mid / value, opr2, rhs / value); }

    Interval differentiate(const Variable& wrt) const { return {lhs.differentiate(wrt), opr1, mid.differentiate(wrt), opr2, rhs.differentiate(wrt)}; }

    std::string to_latex() const {
        return Inequation(lhs, opr1, mid).to_latex().append(" ").append(detail::to_latex(opr2)).append(" ").append(rhs.to_latex());
    }
};

inline algebra::Interval operator<(const algebra::Inequation& lhs, const algebra::Polynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::LT, rhs);
}

inline algebra::Interval operator<=(const algebra::Inequation& lhs, const algebra::Polynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::LE, rhs);
}

inline algebra::Interval operator>(const algebra::Inequation& lhs, const algebra::Polynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::GT, rhs);
}

inline algebra::Interval operator>=(const algebra::Inequation& lhs, const algebra::Polynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::GE, rhs);
}

inline algebra::Interval operator==(const algebra::Inequation& lhs, const algebra::Polynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::EQ, rhs);
}

inline algebra::Interval operator<(const algebra::Polynomial& lhs, const algebra::Inequation& rhs) { return rhs > lhs; }

inline algebra::Interval operator<=(const algebra::Polynomial& lhs, const algebra::Inequation& rhs) { return rhs >= lhs; }

inline algebra::Interval operator>(const algebra::Polynomial& lhs, const algebra::Inequation& rhs) { return rhs < lhs; }

inline algebra::Interval operator>=(const algebra::Polynomial& lhs, const algebra::Inequation& rhs) { return rhs <= lhs; }

inline algebra::Interval operator==(const algebra::Polynomial& lhs, const algebra::Inequation& rhs) { return rhs == lhs; }

inline algebra::Interval operator<(const algebra::Inequation& lhs, const algebra::Fraction& rhs) { return lhs < algebra::Polynomial(rhs); }

inline algebra::Interval operator<=(const algebra::Inequation& lhs, const algebra::Fraction& rhs) { return lhs <= algebra::Polynomial(rhs); }

inline algebra::Interval operator>(const algebra::Inequation& lhs, const algebra::Fraction& rhs) { return lhs > algebra::Polynomial(rhs); }

inline algebra::Interval operator>=(const algebra::Inequation& lhs, const algebra::Fraction& rhs) { return lhs >= algebra::Polynomial(rhs); }

inline algebra::Interval operator==(const algebra::Inequation& lhs, const algebra::Fraction& rhs) { return lhs == algebra::Polynomial(rhs); }

inline algebra::Interval operator<(const algebra::Fraction& lhs, const algebra::Inequation& rhs) { return rhs > lhs; }

inline algebra::Interval operator<=(const algebra::Fraction& lhs, const algebra::Inequation& rhs) { return rhs >= lhs; }

inline algebra::Interval operator>(const algebra::Fraction& lhs, const algebra::Inequation& rhs) { return rhs < lhs; }

inline algebra::Interval operator>=(const algebra::Fraction& lhs, const algebra::Inequation& rhs) { return rhs <= lhs; }

inline algebra::Interval operator==(const algebra::Fraction& lhs, const algebra::Inequation& rhs) { return rhs == lhs; }

inline algebra::Interval operator<(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs < algebra::Polynomial(rhs); }

inline algebra::Interval operator<=(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs <= algebra::Polynomial(rhs); }

inline algebra::Interval operator>(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs > algebra::Polynomial(rhs); }

inline algebra::Interval operator>=(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs >= algebra::Polynomial(rhs); }

inline algebra::Interval operator==(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs == algebra::Polynomial(rhs); }

inline algebra::Interval operator<(const algebra::Variable& lhs, const algebra::Inequation& rhs) { return rhs > lhs; }

inline algebra::Interval operator<=(const algebra::Variable& lhs, const algebra::Inequation& rhs) { return rhs >= lhs; }

inline algebra::Interval operator>(const algebra::Variable& lhs, const algebra::Inequation& rhs) { return rhs < lhs; }

inline algebra::Interval operator>=(const algebra::Variable& lhs, const algebra::Inequation& rhs) { return rhs <= lhs; }

inline algebra::Interval operator==(const algebra::Variable& lhs, const algebra::Inequation& rhs) { return rhs == lhs; }

namespace std {
    inline string to_string(const algebra::Interval& interval) {
        return to_string(algebra::Inequation(interval.lhs, interval.opr1, interval.mid))
            .append(" ")
            .append(to_string(interval.opr2))
            .append(" ")
            .append(to_string(interval.rhs));
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Interval& interval) { return out << std::to_string(interval); }
