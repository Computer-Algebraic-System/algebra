#pragma once
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <utility>
#include <vector>

namespace algebra {
    enum class RelationalOperator { LT, LE, GT, GE, EQ };

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

    namespace detail {
        struct FormatSettings;

        template <typename T>
            requires std::is_same_v<T, Variable> || std::is_same_v<T, Function>
        class AlgebraicExpression;
        template <typename T>
        std::ostream& operator<<(std::ostream&, const AlgebraicExpression<T>&);

        template <typename T>
        class AlgebraicContainer;
        template <typename T>
        std::ostream& operator<<(std::ostream&, const AlgebraicContainer<T>&);

        std::string to_latex(RelationalOperator);
        RelationalOperator invert_relational_operator(RelationalOperator);
        bool evaluate_relational_operator(const Fraction&, RelationalOperator, const Fraction&);
        std::vector<std::vector<int>> generate_combinations(int, int);
    } // namespace detail

    using Polynomial = detail::AlgebraicContainer<Variable>;
    using Expression = detail::AlgebraicContainer<Function>;
} // namespace algebra

#include "utils/detail.hpp"
#include "src/fraction.hpp"
#include "src/complex.hpp"
#include "src/variable.hpp"
#include "utils/algebraic_expression.hpp"
#include "utils/algebraic_container.hpp"
#include "src/function.hpp"
#include "src/inequation.hpp"
#include "src/interval.hpp"
#include "src/graph.hpp"
