#pragma once
#include <algorithm>
#include <assert.h>
#include <cmath>
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

    class Polynomial;
    std::ostream& operator<<(std::ostream&, const Polynomial&);

    class Inequation;
    class Equation;
    std::ostream& operator<<(std::ostream&, const Inequation&);

    class Interval;
    std::ostream& operator<<(std::ostream&, const Interval&);


    class Point;
    class Graph;
    std::ostream& operator<<(std::ostream&, const Point&);

    namespace detail {
        RelationalOperator invert_relational_operator(RelationalOperator);
        bool evaluate_relational_operator(const Fraction&, RelationalOperator, const Fraction&);
        std::vector<std::vector<int>> generate_combinations(int, int);
    } // namespace detail
} // namespace algebra

namespace std {
    inline string to_string(const algebra::RelationalOperator relational_operator) {
        switch (relational_operator) {
        case algebra::RelationalOperator::LT:
            return "<";

        case algebra::RelationalOperator::LE:
            return "<=";

        case algebra::RelationalOperator::GT:
            return ">";

        case algebra::RelationalOperator::GE:
            return ">=";

        case algebra::RelationalOperator::EQ:
            return "=";
        }
        std::unreachable();
    }
} // namespace std

#include "src/fraction.hpp"
#include "src/detail.hpp"
#include "src/variable.hpp"
#include "src/polynomial.hpp"
#include "src/inequation.hpp"
#include "src/interval.hpp"
#include "src/graph.hpp"
