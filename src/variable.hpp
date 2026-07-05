#pragma once

class algebra::Variable {
    static constexpr auto serial_class = detail::SerialClass::VARIABLE;

public:
    struct Var {
        std::string name;
        double exponent;

        std::partial_ordering operator<=>(const Var& value) const { return std::tuple(value.exponent, name) <=> std::tuple(exponent, value.name); }

        bool operator==(const Var&) const = default;
    };
    double coefficient;
    std::vector<Var> variables;

    Variable() = default;

    Variable(const std::string& name) : Variable(1, name) {}

    Variable(const double coefficient) : coefficient(coefficient) {}

    Variable(const double coefficient, const std::string& name) : coefficient(coefficient), variables({{name, 1}}) {}

    Variable operator-() const {
        Variable res = *this;
        res.coefficient = -res.coefficient;
        return res;
    }

    Variable& operator*=(const double value) { return *this *= Variable(value); }

    Variable operator*(const double value) const { return *this * Variable(value); }

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

    Variable& operator/=(const double value) { return *this /= Variable(value); }

    Variable operator/(const double value) const { return *this / Variable(value); }

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

    Variable& operator^=(const double value) {
        coefficient = std::pow(coefficient, value);

        if (value == 0) {
            variables.clear();
        } else {
            for (auto& [_, exponent] : variables) {
                exponent *= value;
            }
        }
        return *this;
    }

    Variable operator^(const double value) const { return Variable(*this) ^= value; }

    std::partial_ordering operator<=>(const Variable& value) const {
        const bool is_const = is_number(), value_const = value.is_number();

        if (is_const != value_const) {
            return is_const ? std::strong_ordering::greater : std::strong_ordering::less;
        }
        return std::tie(variables, coefficient) <=> std::tie(value.variables, value.coefficient);
    }

    bool operator==(const Variable&) const = default;

    Variable basis() const {
        Variable res = *this;
        res.coefficient = 1;
        return res;
    }

    Variable substitute(const std::map<Variable, double>& values, const bool origin = true) const {
        Variable res = *this;

        for (const auto& [variable, value] : values) {
            assert(!variable.variables.empty());
            const std::string& name = variable.variables.front().name;
            const auto itr = std::ranges::find(res.variables, name, &Var::name);

            if (itr != res.variables.end()) {
                res.coefficient *= std::pow(value, itr->exponent);
                res.variables.erase(itr);
            }
            if (res.coefficient == 0) {
                res.variables.clear();
                break;
            }
        }
        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_substitute(*this, values, res);
        }
        return res;
    }

    Variable differentiate(const Variable& wrt, const bool origin = true) const {
        assert(wrt.variables.size() == 1);
        Variable res = *this;
        const auto itr = std::ranges::find(res.variables, wrt.variables.front().name, &Var::name);

        if (itr != res.variables.end()) {
            res.coefficient *= itr->exponent;

            if ((itr->exponent -= 1) == 0) {
                res.variables.erase(itr);
            }
        } else {
            res = {};
        }
        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_differentiate(*this, wrt, res);
        }
        return res;
    }

    Variable integrate(const Variable& wrt, const double a, const double b, const bool origin = true) const {
        assert(wrt.variables.size() == 1);
        static auto tol = std::pow(10, -9);
        int n = 2;
        Variable res, prev;
        std::map<Variable, double> substituent{{wrt, a}}, sub;

        for (const auto& [name, exponent] : variables) {
            sub.emplace(name, 1);
        }
        do {
            const double h = (b - a) / n;
            prev = res;
            substituent.begin()->second = a;
            res = substitute(substituent, false);

            for (int i = 1; i < n; i++) {
                substituent.begin()->second = a + i * h;
                res.coefficient += (i % 2 == 0 ? 2 : 4) * substitute(substituent, false).coefficient;
            }
            substituent.begin()->second = b;
            res.coefficient += substitute(substituent, false).coefficient;
            res *= h / 3;
            n <<= 1;
        } while (std::abs(static_cast<double>(res.substitute(sub, false)) - static_cast<double>(prev.substitute(sub, false))) > tol);
        if (origin && GLOBAL_FORMATTING.verbose) {
            detail::print_integrate(*this, a, b, wrt, res);
        }
        return res;
    }

    bool is_number() const { return variables.empty(); }

    std::string to_latex() const {
        if (variables.empty()) {
            return detail::LaTeX(coefficient).to_latex();
        }
        if (coefficient == 0) {
            return "0";
        }
        std::string res, denominator = "}{";
        const bool fractional =
            std::ranges::find_if(variables, [](const double exponent) -> bool { return exponent < 0; }, &Var::exponent) != variables.end();

        if (fractional) {
            res.append("\\dfrac{");
        }
        if (coefficient == -1) {
            res.push_back('-');
        } else if (std::abs(coefficient) != 1) {
            res.append(detail::LaTeX(coefficient).to_latex());
        }
        for (const auto& [name, exponent] : variables) {
            (exponent < 0 ? denominator : res).append(name.front() == 'L' ? "\\lambda" : std::string(1, name.front()));

            if (name.size() > 1) {
                (exponent < 0 ? denominator : res).append("_{").append(name.substr(1)).push_back('}');
            }
            if (std::abs(exponent) != 1) {
                (exponent < 0 ? denominator : res).append("^{").append(detail::LaTeX(std::abs(exponent)).to_latex()).push_back('}');
            }
        }
        if (fractional) {
            res.append(denominator).push_back('}');
        }
        return res;
    }

    std::string to_html() const {
        if (variables.empty()) {
            return detail::HTML(coefficient).to_html();
        }
        if (coefficient == 0) {
            return "<mn>0</mn>";
        }
        std::string res, denominator;
        const bool fractional =
            std::ranges::find_if(variables, [](const double exponent) -> bool { return exponent < 0; }, &Var::exponent) != variables.end();

        if (coefficient == -1) {
            res.append("<mo>-</mo>");
        } else if (std::abs(coefficient) != 1) {
            res.append(detail::HTML(coefficient).to_html());
        }
        for (const auto& [name, exponent] : variables) {
            std::string var;
            var.append("<mi>").append(name.front() == 'L' ? "&lambda;" : std::string(1, name.front())).append("</mi>");

            if (name.size() > 1) {
                var.insert(0, "<msub>").append("<mn>").append(name.substr(1)).append("</mn></msub>");
            }
            if (std::abs(exponent) != 1) {
                var.insert(0, "<msup>").append(detail::HTML(exponent).to_html()).append("</msup>");
            }
            (exponent < 0 ? denominator : res).append(var);
        }
        if (fractional) {
            res.insert(0, "<mfrac><mrow>").append("</mrow><mrow>").append(denominator).append("</mrow></mfrac>");
        }
        return res;
    }

    constexpr explicit operator double() const {
        assert(is_number());
        return coefficient;
    }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        out.write(reinterpret_cast<const char*>(&coefficient), sizeof(coefficient));
        size_t size = variables.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for (const auto& [name, exponent] : variables) {
            size = name.size();
            out.write(reinterpret_cast<const char*>(&size), sizeof(size));
            out.write(name.c_str(), size);
            out.write(reinterpret_cast<const char*>(&exponent), sizeof(exponent));
        }
    }

    static Variable deserialize(std::ifstream& in) {
        detail::SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        Variable res;
        size_t size;
        in.read(reinterpret_cast<char*>(&res.coefficient), sizeof(res.coefficient));
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        res.variables.resize(size);

        for (size_t i = 0; i < size; i++) {
            size_t len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string name(len, '\0');
            in.read(&name[0], len);
            res.variables[i].name = name;
            in.read(reinterpret_cast<char*>(&res.variables[i].exponent), sizeof(res.variables[i].exponent));
        }
        return res;
    }
};

constexpr algebra::Variable operator*(const double value, const algebra::Variable& variable) { return variable * value; }

constexpr algebra::Variable operator/(const double value, const algebra::Variable& variable) { return algebra::Variable(value) / variable; }

namespace std {
    inline algebra::Variable abs(algebra::Variable variable) {
        variable.coefficient = abs(variable.coefficient);
        return variable;
    }

    inline string to_string(const algebra::Variable& variable) {
        if (variable.variables.empty()) {
            return format(algebra::detail::FORMAT, variable.coefficient);
        }
        if (variable.coefficient == 0) {
            return "0";
        }
        string res, denominator;
        const bool fractional = std::ranges::find_if(
                                    variable.variables, [](const double exponent) -> bool { return exponent < 0; },
                                    &algebra::Variable::Var::exponent) != variable.variables.end();
        const int size = variable.variables.size();

        if (variable.coefficient == -1) {
            res.push_back('-');
        } else if (std::abs(variable.coefficient) != 1) {
            res.append(format(algebra::detail::FORMAT, variable.coefficient));
        }
        for (const auto& [name, exponent] : variable.variables) {
            if (abs(exponent) != 1 && size > 1) {
                (exponent < 0 ? denominator : res).push_back('(');
            }
            (exponent < 0 ? denominator : res).append(name);

            if (abs(exponent) != 1) {
                (exponent < 0 ? denominator : res).append("^").append(format(algebra::detail::FORMAT, exponent));
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
