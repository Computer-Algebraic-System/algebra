#pragma once

class algebra::Variable {
public:
    struct Var {
        std::string name;
        Fraction exponent;

        std::strong_ordering operator<=>(const Var& value) const { return std::tuple(-exponent, name) <=> std::tuple(-value.exponent, value.name); }

        bool operator==(const Var&) const = default;
    };
    Fraction coefficient;
    std::vector<Var> variables;

    Variable() = default;

    Variable(const std::string& name) : Variable(1, name) {}

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
            for (const auto& [name, exponent] : value.variables) {
                const auto itr = std::ranges::find(variables, name, &Var::name);

                if (itr != variables.end()) {
                    if ((itr->exponent += exponent) == 0) {
                        variables.erase(itr);
                    }
                } else {
                    variables.emplace_back(name, exponent);
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

        for (const auto& [name, exponent] : value.variables) {
            const auto itr = std::ranges::find(variables, name, &Var::name);

            if (itr != variables.end()) {
                if ((itr->exponent -= exponent) == 0) {
                    variables.erase(itr);
                }
            } else {
                variables.emplace_back(name, -exponent);
            }
        }
        std::ranges::sort(variables);
        return *this;
    }

    Variable operator/(const Variable& value) const { return Variable(*this) /= value; }

    Variable& operator^=(const Fraction& value) {
        coefficient ^= value;

        if (value == 0) {
            variables.clear();
        } else {
            for (auto& [_, exponent] : variables) {
                exponent *= value;
            }
        }
        return *this;
    }

    Variable operator^(const Fraction& value) const { return Variable(*this) ^= value; }

    std::strong_ordering operator<=>(const Variable& value) const {
        const bool is_const = is_fraction(), value_const = value.is_fraction();

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
            const std::string& name = variable.variables.front().name;
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
        return res;
    }

    Variable differentiate(const Variable& wrt) const {
        assert(!wrt.variables.empty());
        Variable res = *this;
        const auto itr = std::ranges::find(res.variables, wrt.variables.front().name, &Var::name);

        if (itr != res.variables.end()) {
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
        if (variables.empty()) {
            return coefficient.to_latex();
        }
        if (coefficient == 0) {
            return "0";
        }
        std::string res, denominator = "}{";
        const bool fractional = std::ranges::find_if(
                                    variables, [](const Fraction& exponent) -> bool { return exponent < 0; }, &Var::exponent) != variables.end() ||
            coefficient.denominator != 1;

        if (fractional) {
            res.append("\\frac{");
        }
        if (std::abs(coefficient.numerator) == 1) {
            if (coefficient.numerator == -1) {
                res.push_back('-');
            }
        } else {
            res.append(std::to_string(coefficient.numerator));
        }
        if (coefficient.denominator != 1) {
            denominator.append(std::to_string(coefficient.denominator));
        }
        for (const auto& [name, exponent] : variables) {
            (exponent < 0 ? denominator : res).push_back(name.front());

            if (name.size() > 1) {
                (exponent < 0 ? denominator : res).append("_{").append(name.substr(1)).push_back('}');
            }
            if (std::abs(exponent) != 1) {
                (exponent < 0 ? denominator : res).append("^{").append(std::abs(exponent).to_latex()).push_back('}');
            }
        }
        if (fractional) {
            res.append(denominator).push_back('}');
        }
        return res;
    }

    constexpr explicit operator Fraction() const {
        assert(is_fraction());
        return coefficient;
    }
};

constexpr algebra::Variable operator*(const algebra::Fraction& value, const algebra::Variable& variable) { return variable * value; }

constexpr algebra::Variable operator/(const algebra::Fraction& value, const algebra::Variable& variable) {
    return algebra::Variable(value) / variable;
}

namespace std {
    inline algebra::Variable abs(algebra::Variable variable) {
        variable.coefficient = abs(variable.coefficient);
        return variable;
    }

    inline string to_string(const algebra::Variable& variable) {
        if (variable.variables.empty()) {
            return to_string(variable.coefficient);
        }
        if (variable.coefficient == 0) {
            return "0";
        }
        string res, denominator;
        const bool fractional = std::ranges::find_if(
                                    variable.variables, [](const algebra::Fraction& exponent) -> bool { return exponent < 0; },
                                    &algebra::Variable::Var::exponent) != variable.variables.end() ||
            variable.coefficient.denominator != 1;
        const int size = variable.variables.size();

        if (std::abs(variable.coefficient.numerator) == 1) {
            if (variable.coefficient.numerator == -1) {
                res.push_back('-');
            }
        } else {
            res.append(std::to_string(variable.coefficient.numerator));
        }
        if (variable.coefficient.denominator != 1) {
            denominator.append(std::to_string(variable.coefficient.denominator));
        }
        for (const auto& [name, exponent] : variable.variables) {
            if (abs(exponent) != 1 && size > 1) {
                (exponent < 0 ? denominator : res).push_back('(');
            }
            (exponent < 0 ? denominator : res).append(name);

            if (abs(exponent) != 1) {
                (exponent < 0 ? denominator : res).push_back('^');

                if (exponent.denominator != 1) {
                    (exponent < 0 ? denominator : res).push_back('(');
                }
                (exponent < 0 ? denominator : res).append(std::to_string(exponent));

                if (exponent.denominator != 1) {
                    (exponent < 0 ? denominator : res).push_back(')');
                }
            }
            if (abs(exponent) != 1 && size > 1) {
                (exponent < 0 ? denominator : res).push_back(')');
            }
        }
        if (fractional) {
            res.append("/").append(denominator);
        }
        return res;
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Variable& variable) { return out << std::to_string(variable); }
