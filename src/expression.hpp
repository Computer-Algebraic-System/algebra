#pragma once

class algebra::Expression {
public:
    enum class Type { CONSTANT, VARIABLE, ADD, SUB, MUL, DIV, POW, FUNCTION };

private:
    Type type;

    // Data
    Fraction value; // CONSTANT
    std::string name; // VARIABLE / FUNCTION
    std::vector<Expression> args; // children

public:
    // 🔹 Constructors

    Expression() : type(Type::CONSTANT), value(0) {}

    static Expression constant(const Fraction& v) {
        Expression e;
        e.type = Type::CONSTANT;
        e.value = v;
        return e;
    }

    static Expression variable(const std::string& name) {
        Expression e;
        e.type = Type::VARIABLE;
        e.name = name;
        return e;
    }

    static Expression function(const std::string& name, std::vector<Expression> args) {
        Expression e;
        e.type = Type::FUNCTION;
        e.name = name;
        e.args = std::move(args);
        return e;
    }

    static Expression binary(Type t, Expression lhs, Expression rhs) {
        Expression e;
        e.type = t;
        e.args = {std::move(lhs), std::move(rhs)};
        return e;
    }

    // 🔹 Operators (syntactic sugar)

    Expression operator+(Expression other) const { return binary(Type::ADD, *this, std::move(other)); }

    Expression operator-(Expression other) const { return binary(Type::SUB, *this, std::move(other)); }

    Expression operator*(Expression other) const { return binary(Type::MUL, *this, std::move(other)); }

    Expression operator/(Expression other) const { return binary(Type::DIV, *this, std::move(other)); }

    Expression operator^(Expression other) const { return binary(Type::POW, *this, std::move(other)); }

    // 🔹 Debug print (VERY important early on)

    void print(std::ostream& out) const {
        switch (type) {
        case Type::CONSTANT:
            out << value;
            break;

        case Type::VARIABLE:
            out << name;
            break;

        case Type::ADD:
            out << "(";
            args[0].print(out);
            out << " + ";
            args[1].print(out);
            out << ")";
            break;

        case Type::SUB:
            out << "(";
            args[0].print(out);
            out << " - ";
            args[1].print(out);
            out << ")";
            break;

        case Type::MUL:
            out << "(";
            args[0].print(out);
            out << " * ";
            args[1].print(out);
            out << ")";
            break;

        case Type::DIV:
            out << "(";
            args[0].print(out);
            out << " / ";
            args[1].print(out);
            out << ")";
            break;

        case Type::POW:
            out << "(";
            args[0].print(out);
            out << " ^ ";
            args[1].print(out);
            out << ")";
            break;

        case Type::FUNCTION:
            out << name << "(";
            for (size_t i = 0; i < args.size(); ++i) {
                args[i].print(out);
                if (i + 1 < args.size())
                    out << ", ";
            }
            out << ")";
            break;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Expression& e) {
        e.print(os);
        return os;
    }
};
