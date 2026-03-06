#pragma once

class algebra::Graph {
    std::string interpreter_path, source_path;

public:
    Graph(const std::string& interpreter_path = "/home/dream/.virtualenvs/python/bin/python",
          const std::string& source_path = "/home/dream/github/algebra/utils/graph.py") :
        interpreter_path(interpreter_path), source_path(source_path) {}

    void plot(const std::vector<Polynomial>& polynomials, const std::string& variable = "x", const std::string& file_name = "graph.png") const {
        const int size = polynomials.size();
        std::string x;
        std::vector<std::pair<std::string, Fraction>> substituent(1, {variable, 0});
        std::vector<std::string> ys(size);

        for (Fraction i = 0; i < 10; i += Fraction(1, 100)) {
            x += std::to_string(i) + ',';
            substituent.front().second = i;

            for (int j = 0; j < size; j++) {
                ys[j] += std::to_string(polynomials[j].substitute(substituent)) + ',';
            }
        }
        std::string command = interpreter_path + ' ' + source_path + ' ' + file_name + ' ' + x + ' ';

        for (const std::string y : ys) {
            command += y + ' ';
        }
        system(command.c_str());
    }
};
