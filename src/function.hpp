#pragma once

class algebra::Function {
    void simplify() {
        if (coefficient == 0) {
            funcs.clear();
        }
        for (auto& [type, value, exponent] : funcs) {
            if (type == Type::EXP) {
                value *= exponent;
                exponent = 1;
            }
        }
        std::ranges::sort(funcs, std::ranges::greater(), &Func::exponent);
    }

public:
    enum class Type : uint8_t { UNIT, EXP, LN, SIN };
    struct Func {
        Type type;
        Polynomial value;
        Fraction exponent;
    };

    Fraction coefficient;
    std::vector<Func> funcs;

    Function() = default;

    Function(const Fraction& coefficient) : coefficient(coefficient) {}

    Function(const Variable& variable) : Function(Type::UNIT, variable) {}

    Function(const Type type, const Polynomial& value) : coefficient(1), funcs({{type, value, 1}}) { simplify(); }

    Function operator-() const {
        Function res(*this);
        res.coefficient = -res.coefficient;
        return res;
    }

    Function& operator*=(const Fraction& value) { return *this *= Function(value); }

    Function operator*(const Fraction& value) const { return *this * Function(value); }

    Function& operator*=(const Function& value) {
        coefficient *= value.coefficient;
        funcs.insert(funcs.end(), value.funcs.begin(), value.funcs.end());
        return *this;
    }

    Function operator*(const Function& value) const { return Function(*this) *= value; }

    Function& operator/=(const Fraction& value) { return *this /= Function(value); }

    Function operator/(const Fraction& value) const { return *this / Function(value); }

    Function& operator/=(const Function& value) {
        coefficient /= value.coefficient;
        std::ranges::transform(value.funcs, std::back_inserter(funcs), [](Func func) -> Func {
            func.exponent *= -1;
            return func;
        });
        simplify();
        return *this;
    }

    Function operator/(const Function& function) const { return Function(*this) /= function; }

    Function& operator^=(const Fraction& fraction) {
        coefficient ^= fraction;
        std::ranges::for_each(funcs, [&fraction](Func& func) -> void { func.exponent *= fraction; });
        simplify();
        return *this;
    }

    Function operator^(const Fraction& fraction) const { return Function(*this) ^= fraction; }

    Function substitute(const std::vector<std::pair<Variable, Fraction>>& values) const {
        Function res = *this;

        for (Func& func : res.funcs) {
            func.value = func.value.substitute(values);
        }
        return res;
    }

    Expression differentiate(const Variable& parameter) const;

    static Function ln(const Polynomial& polynomial) { return {Type::LN, polynomial}; }

    static Function exp(const Polynomial& polynomial) { return {Type::EXP, polynomial}; }

    static Function sin(const Polynomial& polynomial) { return {Type::SIN, polynomial}; }

    bool is_fraction() const { return funcs.empty(); }

    constexpr explicit operator Fraction() const {
        assert(is_fraction());
        return coefficient;
    }
};

inline algebra::Function operator*(const algebra::Fraction& fraction, const algebra::Function& function) { return function * fraction; }

inline algebra::Function operator/(const algebra::Fraction& fraction, const algebra::Function& function) { return function / fraction; }

namespace std {
    inline algebra::Function abs(algebra::Function function) {
        function.coefficient = abs(function.coefficient);
        return function;
    }

    inline string to_string(const algebra::Function& function) {
        string res;

        if (function.coefficient != 1) {
            res.append(to_string(function.coefficient.numerator));
        }
        for (const auto& [type, value, exponent] : function.funcs) {
            switch (type) {
            case algebra::Function::Type::UNIT:
                res.append(to_string(value));
                break;

            case algebra::Function::Type::EXP:
                res.append("exp");
                break;

            case algebra::Function::Type::LN:
                res.append("ln");
                break;

            case algebra::Function::Type::SIN:
                res.append("sin");
                break;
            }
            if (exponent != 1) {
                res.append("^");

                if (exponent.denominator != 1 || exponent < 0) {
                    res.append("(").append(to_string(exponent)).append(")");
                } else {
                    res.append(to_string(exponent));
                }
            }
            if (type != algebra::Function::Type::UNIT) {
                res.append("(").append(to_string(value)).append(")");
            }
        }
        if (function.coefficient.denominator != 1) {
            res.append("/").append(to_string(function.coefficient.denominator));
        }
        return res;
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Function& function) { return out << std::to_string(function); }
