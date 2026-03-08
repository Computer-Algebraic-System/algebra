#pragma once

class algebra::Graph {
public:
    inline static std::string interpreter_path = "/home/dream/.virtualenvs/python/bin/python";
    inline static std::string source_path = "/home/dream/github/algebra/utils/graph.py";

    static void plot(const std::vector<Inequation>& inequations, const std::string& variable = "x", const std::string& file_name = "graph.png") {
        const int size = inequations.size();
        std::string x;
        std::vector<Polynomial> simplified;
        std::vector<std::pair<std::string, Fraction>> substituent(1, {variable, 0});
        std::vector<std::vector<int>> combinations = detail::generate_combinations(size, 2);
        std::vector<std::string> ys(size);
        simplified.reserve(size);

        for (const Inequation& inequation : inequations) {
            simplified.push_back(inequation.solve_for(Variable("y")).rhs);
        }
        for (Fraction i = 0; i < 10; i += Fraction(1, 100)) {
            x.append(std::to_string(i)).push_back(',');
            substituent[0].second = i;

            for (int j = 0; j < size; j++) {
                ys[j].append(std::to_string(simplified[j].substitute(substituent))).push_back(',');
            }
        }
        for (int i = 0; i < size; i++) {
            ys[i].append(" '").append(std::to_string(inequations[i])).push_back('\'');
        }
        std::string command = interpreter_path + ' ' + source_path + ' ' + file_name + ' ' + x + ' ';

        for (const std::string y : ys) {
            command += y + ' ';
        }
        system(command.c_str());
    }
};
