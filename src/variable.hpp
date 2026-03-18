#pragma once

class algebra::Variable {
    struct Var {
        std::string name;
        Fraction exponent;

        std::strong_ordering operator<=>(const Var& value) const { return std::tuple(-exponent, name) <=> std::tuple(-value.exponent, value.name); }

        bool operator==(const Var&) const = default;
    };

public:
    Fraction coefficient;
    std::vector<Var> variables;

    Variable() = default;

    Variable(const std::string& name) : coefficient(1), variables({{name, 1}}) {}

    Variable(const Fraction& coefficient) : coefficient(coefficient) {}

    Variable(const Fraction& coefficient, const std::string& name) : coefficient(coefficient), variables({{name, 1}}) {}

    Variable operator-() const {
        Variable res = *this;
        res.coefficient = -res.coefficient;
        return res;
    }

    Variable& operator*=(const Fraction& value) { return *this *= Variable(value); }

    Variable operator*(const Fraction& value) const { return *this * Variable(value); }

    Variable& operator*=(const Variable& value) {
        coefficient *= value.coefficient;

        if (coefficient == 0) {
            variables.clear();
        } else {
            for (const Var& var : value.variables) {
                const auto itr = std::ranges::find(variables, var.name, &Var::name);

                if (itr != variables.end()) {
                    if ((itr->exponent += var.exponent) == 0) {
                        variables.erase(itr);
                    }
                } else {
                    variables.push_back(var);
                }
            }
            std::ranges::sort(variables);
        }
        return *this;
    }

    Variable operator*(const Variable& value) const { return Variable(*this) *= value; }

    Variable& operator/=(const Fraction& value) { return *this /= Variable(value); }

    Variable operator/(const Fraction& value) const { return *this / Variable(value); }

    Variable& operator/=(const Variable& value) {
        coefficient /= value.coefficient;

        for (const Var& var : value.variables) {
            const auto itr = std::ranges::find(variables, var.name, &Var::name);

            if (itr != variables.end()) {
                if ((itr->exponent -= var.exponent) == 0) {
                    variables.erase(itr);
                }
            } else {
                variables.emplace_back(var.name, -var.exponent);
            }
        }
        std::ranges::sort(variables);
        return *this;
    }

    Variable operator/(const Variable& value) const { return Variable(*this) /= value; }

    Variable& operator^=(const Fraction& value) {
        coefficient ^= value;

        for (auto& [_, exponent] : variables) {
            exponent *= value;
        }
        return *this;
    }

    Variable operator^(const Fraction& value) const { return Variable(*this) ^= value; }

    std::strong_ordering operator<=>(const Variable& value) const {
        const bool is_const = variables.empty(), value_const = value.variables.empty();

        if (is_const != value_const) {
            return is_const ? std::strong_ordering::greater : std::strong_ordering::less;
        }
        return std::tie(variables, coefficient) <=> std::tie(value.variables, value.coefficient);
    }

    bool operator==(const Variable&) const = default;

    Variable substitute(const std::vector<std::pair<Variable, Fraction>>& values) const {
        Variable res = *this;

        for (const auto& [variable, value] : values) {
            assert(!variable.variables.empty());
            const std::string name = variable.variables.front().name;
            const auto itr = std::ranges::lower_bound(res.variables, name, {}, &Var::name);

            if (itr != res.variables.end() && itr->name == name) {
                res.coefficient *= value ^ itr->exponent;
                res.variables.erase(itr);
            }
            if (res.coefficient == 0) {
                res.variables.clear();
                break;
            }
        }
        return res;
    }

    Variable basis() const {
        Variable res = *this;
        res.coefficient = 1;

        for (auto& [_, exponent] : res.variables) {
            exponent = 1;
        }
        return res;
    }

    Variable differentiate(const Variable& wrt) const {
        Variable res = *this;
        const auto itr = std::ranges::find(res.variables, wrt.variables.front().name, &Var::name);

        if (itr != res.variables.end() && itr->name == wrt.variables.front().name) {
            res.coefficient *= itr->exponent;

            if ((itr->exponent -= 1) == 0) {
                res.variables.erase(itr);
            }
            return res;
        }
        return {};
    }

    bool is_fraction() const { return variables.empty(); }

    std::string to_latex() const {
        auto convert = [this]() -> std::string {
            std::string res;

            for (const auto& [name, exponent] : variables) {
                res.push_back(name.front());
                res.append("_{").append(name.substr(1)).push_back('}');

                if (exponent != 1) {
                    res.append("^{").append(exponent.to_latex()).push_back('}');
                }
            }
            return res;
        };
        if (variables.empty()) {
            return coefficient.to_latex();
        }
        if (coefficient == 0) {
            return "0";
        }
        if (coefficient == 1) {
            return convert();
        }
        if (coefficient == -1) {
            return '-' + convert();
        }
        if (coefficient.denominator != 1) {
            std::string res("\\frac{");

            if (std::abs(coefficient.numerator) == 1) {
                if (coefficient.numerator == -1) {
                    res.push_back('-');
                }
            } else {
                res.append(std::to_string(coefficient.numerator));
            }
            res.append(convert()).append("}{").append(std::to_string(coefficient.denominator)).push_back('}');
            return res;
        }
        return coefficient.to_latex() + convert();
    }

    constexpr explicit operator Fraction() const {
        assert(is_fraction());
        return coefficient;
    }
};

constexpr algebra::Variable operator*(const algebra::Fraction& value, const algebra::Variable& variable) { return variable * value; }

constexpr algebra::Variable operator/(const algebra::Fraction& value, const algebra::Variable& variable) { return variable / value; }

namespace std {
    inline algebra::Variable abs(algebra::Variable variable) {
        variable.coefficient = abs(variable.coefficient);
        return variable;
    }

    inline string to_string(const algebra::Variable& variable) {
        auto convert = [](const algebra::Variable& var) -> string {
            const int size = var.variables.size();
            std::string res;

            for (const auto& [name, exponent] : var.variables) {
                if (exponent != 1 && size > 1) {
                    res.push_back('(');
                }
                res.append(name);

                if (exponent != 1) {
                    res.push_back('^');

                    if (exponent.denominator != 1) {
                        res.push_back('(');
                    }
                    res.append(std::to_string(exponent));

                    if (exponent.denominator != 1) {
                        res.push_back(')');
                    }
                }
                if (exponent != 1 && size > 1) {
                    res.push_back(')');
                }
            }
            return res;
        };
        if (variable.variables.empty()) {
            return to_string(variable.coefficient);
        }
        if (variable.coefficient == 0) {
            return "0";
        }
        if (variable.coefficient == 1) {
            return convert(variable);
        }
        if (variable.coefficient == -1) {
            return '-' + convert(variable);
        }
        if (variable.coefficient.denominator != 1) {
            string res;

            if (abs(variable.coefficient.numerator) == 1) {
                if (variable.coefficient.numerator == -1) {
                    res.push_back('-');
                }
            } else {
                res.append(to_string(variable.coefficient.numerator));
            }
            res.append(convert(variable)).append("/").append(to_string(variable.coefficient.denominator));
            return res;
        }
        return to_string(variable.coefficient) + convert(variable);
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Variable& variable) { return out << std::to_string(variable); }
