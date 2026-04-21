#pragma once
#include <algorithm>
#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include <complex>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <ranges>
#include <utility>
#include <vector>

namespace algebra {
    enum class RelationalOperator : uint8_t { LT, LE, GT, GE, EQ, NE };

    class BigInt;
    std::ostream& operator<<(std::ostream&, const BigInt&);

    class Fraction;
    std::ostream& operator<<(std::ostream&, const Fraction&);

    class Complex;
    std::ostream& operator<<(std::ostream&, const Complex&);

    class Variable;
    std::ostream& operator<<(std::ostream&, const Variable&);

    class Function;
    std::ostream& operator<<(std::ostream&, const Function&);

    class Inequation;
    class Equation;
    std::ostream& operator<<(std::ostream&, const Inequation&);

    class Interval;
    std::ostream& operator<<(std::ostream&, const Interval&);

    class Point;
    class Graph;
    std::ostream& operator<<(std::ostream&, const Point&);

    class Definition;
    std::ostream& operator<<(std::ostream&, const Definition&);

    namespace detail {
        class LaTeX;
        std::ostream& operator<<(std::ostream&, const LaTeX&);

        class HTML;
        std::ostream& operator<<(std::ostream&, const HTML&);

        struct FormatSettings;

        template <typename T>
            requires std::is_same_v<T, Variable> || std::is_same_v<T, Function>
        class AlgebraicExpression;
        template <typename T>
        std::ostream& operator<<(std::ostream&, const AlgebraicExpression<T>&);

        template <typename T>
            requires std::is_same_v<T, Variable> || std::is_same_v<T, Function>
        class AlgebraicContainer;
        template <typename T>
        std::ostream& operator<<(std::ostream&, const AlgebraicContainer<T>&);

        std::string to_latex(RelationalOperator);
        RelationalOperator invert_relational_operator(RelationalOperator);
        bool evaluate_relational_operator(const Fraction&, RelationalOperator, const Fraction&);
        std::vector<std::vector<int>> generate_combinations(int, int);

        template <typename T, typename U, typename V>
        void print_substitute(const T&, const std::map<U, Fraction>&, const V&);
        template <typename T, typename U, typename V>
        void print_differentiate(const T&, const U&, const V&);
        template <typename T, typename U, typename V, typename W>
        void print_integrate(const T&, const U&, const U&, const V&, const W&);
    } // namespace detail

    using SimplePolynomial = detail::AlgebraicExpression<Variable>;
    using RationalPolynomial = detail::AlgebraicContainer<Variable>;
    using SimpleExpression = detail::AlgebraicExpression<Function>;
    using RationalExpression = detail::AlgebraicContainer<Function>;

    namespace detail {
        enum class SerialClass : uint8_t {
            BIG_INT,
            FRACTION,
            COMPLEX,
            VARIABLE,
            FUNCTION,
            INEQUATION,
            INTERVAL,
            POINT,
            SIMPLE_POLYNOMIAL,
            RATIONAL_POLYNOMIAL,
            SIMPLE_EXPRESSION,
            RATIONAL_EXPRESSION,
            DEFINITION,
        };
    } // namespace detail
} // namespace algebra

#include "utils/detail.hpp"
#include "src/big_int.hpp"
#include "src/fraction.hpp"
#include "src/complex.hpp"
#include "src/variable.hpp"
#include "utils/algebraic_expression.hpp"
#include "utils/algebraic_container.hpp"
// #include "src/function.hpp"
#include "src/inequation.hpp"
#include "src/interval.hpp"
#include "src/graph.hpp"
#include "src/defination.hpp"
