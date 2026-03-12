#pragma once

namespace algebra::detail {
    inline RelationalOperator invert_relational_operator(const RelationalOperator opr) {
        switch (opr) {
        case RelationalOperator::LT:
            return RelationalOperator::GT;

        case RelationalOperator::LE:
            return RelationalOperator::GE;

        case RelationalOperator::GT:
            return RelationalOperator::LT;

        case RelationalOperator::GE:
            return RelationalOperator::LE;

        default:
            return RelationalOperator::EQ;
        }
    }

    inline bool evaluate_relational_operator(const Fraction& lhs, const RelationalOperator opr, const Fraction& rhs) {
        switch (opr) {
        case RelationalOperator::LT:
            return lhs < rhs;

        case RelationalOperator::LE:
            return lhs <= rhs;

        case RelationalOperator::GT:
            return lhs > rhs;

        case RelationalOperator::GE:
            return lhs >= rhs;

        default:
            return lhs == rhs;
        }
    }

    inline std::vector<std::vector<int>> generate_combinations(const int n, const int k) {
        std::vector<int> current;
        std::vector<std::vector<int>> res;

        const auto inner = [](auto&& self, const int start, const int a, const int b, std::vector<int>& curr,
                              std::vector<std::vector<int>>& ans) -> void {
            if (curr.size() == b) {
                ans.push_back(curr);
                return;
            }

            for (int i = start; i < a; i++) {
                curr.push_back(i);
                self(self, i + 1, a, b, curr, ans);
                curr.pop_back();
            }
        };
        inner(inner, 0, n, k, current, res);
        return res;
    }
} // namespace algebra::detail
