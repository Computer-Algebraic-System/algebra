#pragma once
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <utility>
#include <vector>

namespace algebra {
    using int128_t = __int128;

    enum class RelationalOperator { LT, LE, GT, GE, EQ };

    class Fraction;
    std::ostream& operator<<(std::ostream&, const Fraction&);

    class Variable;
    std::ostream& operator<<(std::ostream&, const Variable&);

    template <typename T>
    class AlgebraicContainer;
    template <typename T>
    std::ostream& operator<<(std::ostream&, const AlgebraicContainer<T>&);

    using Polynomial = AlgebraicContainer<Variable>;

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

        std::string to_latex(RelationalOperator);
        RelationalOperator invert_relational_operator(RelationalOperator);
        bool evaluate_relational_operator(const Fraction&, RelationalOperator, const Fraction&);
        std::vector<std::vector<int>> generate_combinations(int, int);
    } // namespace detail
} // namespace algebra

#include "src/detail.hpp"
#include "src/fraction.hpp"
#include "src/variable.hpp"
#include "src/algebraic_container.hpp"
#include "src/inequation.hpp"
#include "src/interval.hpp"
#include "src/graph.hpp"
