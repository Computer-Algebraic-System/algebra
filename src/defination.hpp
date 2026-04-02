#pragma once

class algebra::Definition {
    Definition() = default;

public:
    static constexpr auto serial_class = detail::SerialClass::DEFINITION;
    std::vector<Variable> function_of;
    std::vector<std::pair<RationalPolynomial, std::string>> definitions;

    explicit Definition(const std::vector<Variable>& function_of, const std::vector<std::pair<RationalPolynomial, std::string>>& definitions) :
        function_of(function_of), definitions(definitions) {
        assert(!function_of.empty());
    }

    std::string to_latex() const {
        std::string res = "f(";
        res.append(function_of.front().to_latex());

        for (const Variable& variable : function_of | std::views::drop(1)) {
            res.append(",").append(variable.to_latex());
        }
        res.append(")=\n\\begin{cases}\n");

        for (const auto& [lhs, rhs] : definitions) {
            res.append(lhs.to_latex()).append(" & \\quad ").append(rhs).append("\\\\[10pt]\n");
        }
        return res.append("\\end{cases}\n");
    }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        size_t size = function_of.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for (const Variable& variable : function_of) {
            variable.serialize(out);
        }
        size = definitions.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for (const auto& [lhs, rhs] : definitions) {
            lhs.serialize(out);
            size = rhs.size();
            out.write(reinterpret_cast<const char*>(&size), sizeof(size));
            out.write(rhs.c_str(), size);
        }
    }

    static Definition deserialize(std::ifstream& in) {
        detail::SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        Definition definition;
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        definition.function_of.reserve(size);

        for (int i = 0; i < size; i++) {
            definition.function_of.push_back(Variable::deserialize(in));
        }
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        definition.definitions.reserve(size);

        for (int i = 0; i < size; i++) {
            size_t len;
            RationalPolynomial lhs = RationalPolynomial::deserialize(in);
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            std::string rhs(len, '\0');
            in.read(rhs.data(), len);
            definition.definitions.emplace_back(lhs, rhs);
        }
        return definition;
    }
};

namespace std {
    inline string to_string(const algebra::Definition& definition) {
        string res, name = "f(";
        name.append(to_string(definition.function_of.front()));

        for (const algebra::Variable& variable : definition.function_of | views::drop(1)) {
            name.append(", ").append(to_string(variable));
        }
        name.append(") = ");
        const int size = definition.definitions.size(), mid = size / 2;
        int max_align = 0;
        const int alignment = name.size();
        vector<string> lhs_s;
        lhs_s.reserve(size);

        for (const algebra::RationalPolynomial& lhs : definition.definitions | views::keys) {
            lhs_s.push_back(to_string(lhs));
            max_align = max(max_align, static_cast<int>(lhs_s.back().size()));
        }
        for (int i = 0; i < size; i++) {
            if (i == mid) {
                res.append(name);
            } else {
                res.append(alignment, ' ');
            }
            res.append("{ ").append(lhs_s[i]).append(max_align - lhs_s[i].size() + 4, ' ').append(definition.definitions[i].second).push_back('\n');
        }
        return res;
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Definition& definition) { return out << std::to_string(definition); }
