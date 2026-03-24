#pragma once

namespace algebra {
    namespace detail {
        struct FormatSettings {
            enum class Output { CONSOLE, FILE, LATEX } output = Output::CONSOLE;
            std::ofstream file;
            std::string filename;

            void toggle_file(const std::string& name) {
                output = Output::FILE;
                filename = name;
                file.open(filename);
            }

            void toggle_latex(const std::string& name) {
                output = Output::LATEX;
                filename = name;
                file.open(filename);
                *this << "\\documentclass{article}\n";
                *this << "\\usepackage{amsmath}\n";
                *this << "\\usepackage{graphicx}\n";
                *this << "\\renewcommand{\\arraystretch}{1.5}\n";
                *this << "\\begin{document}\n";
            }

            template <typename T>
            friend FormatSettings& operator<<(FormatSettings& fmt, const T& object) {
                switch (fmt.output) {
                case Output::CONSOLE:
                    std::cout << object;
                    break;

                case Output::LATEX:
                    if constexpr (requires(const T& obj) { obj.to_latex(); }) {
                        fmt.file << "\\begin{align*}\n" << object.to_latex() << "\\end{align*}\n";
                    } else {
                        fmt.file << object;
                    }
                    break;

                case Output::FILE:
                    fmt.file << object;
                    break;
                }
                return fmt;
            }

            friend FormatSettings& operator<<(FormatSettings& fmt, std::ostream& (*manip)(std::ostream&)) {
                switch (fmt.output) {
                case Output::CONSOLE:
                    manip(std::cout);
                    break;

                case Output::LATEX:
                    fmt.file << "\\\\\n";
                    break;

                case Output::FILE:
                    manip(fmt.file);
                    break;
                }
                return fmt;
            }

            ~FormatSettings() {
                switch (output) {
                case Output::FILE:
                    file.close();
                    break;

                case Output::LATEX:
                    {
                        *this << "\\end{document}\n";
                        file.close();
                        std::string base = filename.substr(0, filename.size() - 4);
                        std::string command("pdflatex -interaction=nonstopmode ");
                        command.append(filename).append(" > /dev/null 2>&1 && rm -f ").append(base).append(".log ").append(base).append(".aux");
                        system(command.c_str());
                        break;
                    }

                case Output::CONSOLE:
                    break;
                }
            }
        }; // namespace detail

        inline std::string to_latex(const RelationalOperator opr) {
            switch (opr) {
            case RelationalOperator::LT:
                return "<";

            case RelationalOperator::LE:
                return "\\le";

            case RelationalOperator::GT:
                return ">";

            case RelationalOperator::GE:
                return "\\ge";

            case RelationalOperator::EQ:
                return "=";
            }
            std::unreachable();
        }

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
    } // namespace detail

    inline detail::FormatSettings GLOBAL_FORMATTING;
} // namespace algebra

namespace std {
    inline string to_string(const algebra::RelationalOperator opr) {
        switch (opr) {
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
