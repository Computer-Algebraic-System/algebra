#pragma once

class algebra::Point {
public:
    Fraction x, y;

    std::strong_ordering operator<=>(const Point&) const = default;
};

namespace std {
    inline string to_string(const algebra::Point& point) {
        return string("(").append(to_string(point.x)).append(", ").append(to_string(point.y)).append(")");
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Point& point) { return out << std::to_string(point); }

class algebra::Graph {
public:
    inline static std::string interpreter_path = "/home/dream/.virtualenvs/python/bin/python";
    inline static std::string source_path = "/home/dream/github/algebra/utils/graph.py";

    // 0 -> Bounded | 1 -> Unbounded
    // filename x n y1 ie1 y2 ie2 ... yn ien m p1 p2 ... pm k x1 ie1 x2 ie2 ... xk iek
    static int plot(const std::vector<Inequation>& inequations, const std::vector<Point>& points = {}, const Fraction& limit = 10,
                    const std::string& file_name = "graph.png") {
        const int inequations_size = inequations.size(), points_size = points.size();
        const Fraction increment = limit / 100;
        int actual_inequation_size = inequations_size;
        std::string x;
        std::vector<Polynomial> simplified(inequations_size);
        std::vector<Fraction> non_ys(inequations_size);
        std::vector<std::pair<Variable, Fraction>> substituent(1, {Variable("x"), 0});
        std::vector<std::vector<int>> combinations = detail::generate_combinations(inequations_size, 2);
        std::vector<std::string> ys(inequations_size);

        for (int i = 0; i < inequations_size; i++) {
            if (std::ranges::contains(std::array{inequations[i].lhs.numerator.terms, inequations[i].rhs.numerator.terms} | std::views::join,
                                      Variable("y"), &Variable::basis)) {
                simplified[i] = inequations[i].solve_for(Variable("y")).rhs;
                non_ys[i] = inf;
            } else {
                non_ys[i] = static_cast<Fraction>(inequations[i].solve_for(Variable("x")).rhs);
                actual_inequation_size--;
            }
        }
        for (Fraction i = 0; i < limit; i += increment) {
            x.append(std::to_string(i)).push_back(',');
            substituent[0].second = i;

            for (int j = 0; j < inequations_size; j++) {
                if (non_ys[j] == inf) {
                    ys[j].append(std::to_string(simplified[j].substitute(substituent))).push_back(',');
                }
            }
        }
        for (int i = 0; i < inequations_size; i++) {
            ys[i].append(" '").append(std::to_string(inequations[i])).push_back('\'');
        }
        std::string command(interpreter_path);
        command.append(" ")
            .append(source_path)
            .append(" ")
            .append(file_name)
            .append(" ")
            .append(x)
            .append(" ")
            .append(std::to_string(actual_inequation_size))
            .push_back(' ');

        for (int i = 0; i < inequations_size; i++) {
            if (non_ys[i] == inf) {
                command.append(ys[i]).push_back(' ');
            }
        }
        command.append(std::to_string(points_size)).push_back(' ');

        for (const auto& [px, py] : points) {
            command.append(std::to_string(px)).append(",").append(std::to_string(py)).push_back(' ');
        }
        command.append(std::to_string(inequations_size - actual_inequation_size)).push_back(' ');

        for (int i = 0; i < inequations_size; i++) {
            if (non_ys[i] != inf) {
                command.append(std::to_string(non_ys[i])).append(" ").append(ys[i]).push_back(' ');
            }
        }
        const int code = system(command.c_str());

        if (GLOBAL_FORMATTING.output == detail::FormatSettings::Output::LATEX) {
            std::string latex("\\begin{center}\n\\includegraphics[width=\\textwidth]{");
            latex.append(file_name).append("}\n\\end{center}\n");
            GLOBAL_FORMATTING << latex;
        }
        return code;
    }
};
