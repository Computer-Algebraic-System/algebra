#pragma once

class algebra::Interval {
    static constexpr auto serial_class = detail::SerialClass::INTERVAL;

public:
    SimplePolynomial lhs, mid, rhs;
    RelationalOperator opr1, opr2;

    Interval() = default;

    Interval(const SimplePolynomial& lhs, const RelationalOperator opr1, const SimplePolynomial& mid, const RelationalOperator opr2,
             const SimplePolynomial& rhs) : lhs(lhs), mid(mid), rhs(rhs), opr1(opr1), opr2(opr2) {}

    Interval(const Inequation& lhs, const RelationalOperator opr, const SimplePolynomial& rhs) :
        lhs(lhs.lhs), mid(lhs.rhs), rhs(rhs), opr1(lhs.opr), opr2(opr) {}

    Interval& operator+=(const double value) { return *this += SimplePolynomial(value); }

    Interval operator+(const double value) const { return *this + SimplePolynomial(value); }

    Interval& operator+=(const Variable& value) { return *this += SimplePolynomial(value); }

    Interval operator+(const Variable& value) const { return *this + SimplePolynomial(value); }

    Interval& operator+=(const SimplePolynomial& value) { return *this = *this + value; }

    Interval operator+(const SimplePolynomial& value) const { return Interval(lhs + value, opr1, mid + value, opr2, rhs + value); }

    Interval& operator-=(const double value) { return *this += -value; }

    Interval operator-(const double value) const { return *this + -value; }

    Interval& operator-=(const Variable& value) { return *this += -value; }

    Interval operator-(const Variable& value) const { return *this + -value; }

    Interval& operator-=(const SimplePolynomial& value) { return *this += -value; }

    Interval operator-(const SimplePolynomial& value) const { return *this + -value; }

    Interval& operator*=(const double value) { return *this *= SimplePolynomial(value); }

    Interval operator*(const double value) const { return *this * SimplePolynomial(value); }

    Interval& operator*=(const Variable& value) { return *this *= SimplePolynomial(value); }

    Interval operator*(const Variable& value) const { return *this * SimplePolynomial(value); }

    Interval& operator*=(const SimplePolynomial& value) { return *this = *this * value; }

    Interval operator*(const SimplePolynomial& value) const { return Interval(lhs * value, opr1, mid * value, opr2, rhs * value); }

    Interval& operator/=(const double value) { return *this /= Variable(value); }

    Interval operator/(const double value) const { return *this / Variable(value); }

    Interval& operator/=(const Variable& value) { return *this = *this / value; }

    Interval operator/(const Variable& value) const { return Interval(lhs / value, opr1, mid / value, opr2, rhs / value); }

    Interval differentiate(const Variable& wrt, const bool origin = true) const {
        Interval res(lhs.differentiate(wrt, false), opr1, mid.differentiate(wrt, false), opr2, rhs.differentiate(wrt, false));

        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_differentiate(*this, wrt, res);
        }
        return res;
    }

    Interval integral(const Variable& wrt, const double a, const double b, const bool origin = true) const {
        Interval res(lhs.integrate(wrt, a, b), opr1, mid.integrate(wrt, a, b), opr2, rhs.integrate(wrt, a, b));

        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_integrate(*this, a, b, wrt, res);
        }
        return res;
    }

    std::string to_latex() const { return Inequation(lhs, opr1, mid).to_latex().append(detail::to_latex(opr2)).append(rhs.to_latex()); }

    std::string to_html() const { return Inequation(lhs, opr1, mid).to_html().append(detail::to_html(opr2)).append(rhs.to_html()); }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        lhs.serialize(out);
        out.write(reinterpret_cast<const char*>(&opr1), sizeof(opr1));
        mid.serialize(out);
        out.write(reinterpret_cast<const char*>(&opr2), sizeof(opr2));
        rhs.serialize(out);
    }

    static Interval deserialize(std::ifstream& in) {
        detail::SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        Interval res;
        res.lhs = SimplePolynomial::deserialize(in);
        in.read(reinterpret_cast<char*>(&res.opr1), sizeof(res.opr1));
        res.mid = SimplePolynomial::deserialize(in);
        in.read(reinterpret_cast<char*>(&res.opr2), sizeof(res.opr2));
        res.rhs = SimplePolynomial::deserialize(in);
        return res;
    }
};

inline algebra::Interval operator<(const algebra::Inequation& lhs, const algebra::SimplePolynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::LT, rhs);
}

inline algebra::Interval operator<=(const algebra::Inequation& lhs, const algebra::SimplePolynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::LE, rhs);
}

inline algebra::Interval operator>(const algebra::Inequation& lhs, const algebra::SimplePolynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::GT, rhs);
}

inline algebra::Interval operator>=(const algebra::Inequation& lhs, const algebra::SimplePolynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::GE, rhs);
}

inline algebra::Interval operator==(const algebra::Inequation& lhs, const algebra::SimplePolynomial& rhs) {
    return algebra::Interval(lhs, algebra::RelationalOperator::EQ, rhs);
}

inline algebra::Interval operator<(const algebra::SimplePolynomial& lhs, const algebra::Inequation& rhs) { return rhs > lhs; }

inline algebra::Interval operator<=(const algebra::SimplePolynomial& lhs, const algebra::Inequation& rhs) { return rhs >= lhs; }

inline algebra::Interval operator>(const algebra::SimplePolynomial& lhs, const algebra::Inequation& rhs) { return rhs < lhs; }

inline algebra::Interval operator>=(const algebra::SimplePolynomial& lhs, const algebra::Inequation& rhs) { return rhs <= lhs; }

inline algebra::Interval operator==(const algebra::SimplePolynomial& lhs, const algebra::Inequation& rhs) { return rhs == lhs; }

inline algebra::Interval operator<(const algebra::Inequation& lhs, const double rhs) { return lhs < algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator<=(const algebra::Inequation& lhs, const double rhs) { return lhs <= algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator>(const algebra::Inequation& lhs, const double rhs) { return lhs > algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator>=(const algebra::Inequation& lhs, const double rhs) { return lhs >= algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator==(const algebra::Inequation& lhs, const double rhs) { return lhs == algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator<(const double lhs, const algebra::Inequation& rhs) { return rhs > lhs; }

inline algebra::Interval operator<=(const double lhs, const algebra::Inequation& rhs) { return rhs >= lhs; }

inline algebra::Interval operator>(const double lhs, const algebra::Inequation& rhs) { return rhs < lhs; }

inline algebra::Interval operator>=(const double lhs, const algebra::Inequation& rhs) { return rhs <= lhs; }

inline algebra::Interval operator==(const double lhs, const algebra::Inequation& rhs) { return rhs == lhs; }

inline algebra::Interval operator<(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs < algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator<=(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs <= algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator>(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs > algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator>=(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs >= algebra::SimplePolynomial(rhs); }

inline algebra::Interval operator==(const algebra::Inequation& lhs, const algebra::Variable& rhs) { return lhs == algebra::SimplePolynomial(rhs); }

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
