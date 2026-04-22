#pragma once

namespace algebra {
    namespace detail {
        class LaTeX {
            const std::string& latex;

        public:
            LaTeX(const std::string& latex) : latex(latex) {}

            const std::string& to_latex() const { return latex; }
        };

        inline std::ostream& operator<<(std::ostream& out, const LaTeX& latex) { return out << latex.to_latex(); }

        class HTML {
            const std::string& html;

        public:
            HTML(const std::string& html) : html(html) {}

            const std::string& to_html() const { return html; }
        };

        inline std::ostream& operator<<(std::ostream& out, const HTML& html) { return out << html.to_html(); }

        struct FormatSettings {
            enum class Output { CONSOLE, FILE, LATEX, HTML } output = Output::CONSOLE;
            bool was_math = false, verbose = true;
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
                file << "\\documentclass{article}\n\\usepackage[fontsize=8.5pt]{fontsize}\n\\usepackage[top=0.5in, bottom=1in, left=0.5in, "
                        "right=0.5in]D{geometry}\n\\usepackage{amsmath}\n\\usepackage{graphicx}\n\\renewcommand{\\arraystretch}{1.5}\n\\begin{"
                        "document}\n";
            }

            void toggle_html(const std::string& name) {
                output = Output::HTML;
                filename = name;
                file.open(filename);
                file << "<html>\n<head>\n<style>\nmath[display='block'] {\nmargin-top: 35px;\nmargin-bottom: 35px;\n}\n</style>\n</head>\n<body>\n";
            }

            template <typename T>
            friend FormatSettings& operator<<(FormatSettings& fmt, const T& object) {
                fmt.was_math = false;

                if (fmt.verbose) {
                    switch (fmt.output) {
                    case Output::CONSOLE:
                        std::cout << object;
                        break;

                    case Output::LATEX:
                        if constexpr (requires(const T& obj) { obj.to_latex(); }) {
                            fmt.file << "\\begin{align*}\n" << object.to_latex() << "\\end{align*}\n";
                            fmt.was_math = true;
                        } else {
                            fmt.file << object;
                        }
                        break;

                    case Output::HTML:
                        if constexpr (requires(const T& obj) { obj.to_html(); }) {
                            fmt.file << "<math display='block'>\n" << object.to_html() << "\n</math>";
                        } else {
                            fmt.file << object;
                        }
                        break;

                    case Output::FILE:
                        fmt.file << object;
                        break;
                    }
                }
                return fmt;
            }

            friend FormatSettings& operator<<(FormatSettings& fmt, std::ostream& (*manip)(std::ostream&)) {
                if (fmt.verbose) {
                    switch (fmt.output) {
                    case Output::CONSOLE:
                        manip(std::cout);
                        break;

                    case Output::LATEX:
                        if (!fmt.was_math) {
                            fmt.file << "\\\\\n";
                        }
                        break;

                    case Output::HTML:
                    case Output::FILE:
                        manip(fmt.file);
                        break;
                    }
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
                        file << "\\end{document}\n";
                        file.close();
                        const std::string base = filename.substr(0, filename.size() - 4);
                        std::string command("pdflatex -interaction=nonstopmode ");
                        system(command.append(filename)
                                   .append(" > /dev/null 2>&1 && rm -f ")
                                   .append(base)
                                   .append(".log ")
                                   .append(base)
                                   .append(".aux ")
                                   .append(base)
                                   .append(".tex")
                                   .c_str());
                        break;
                    }

                case Output::HTML:
                    file << "</body>\n</html>\n";
                    file.close();
                    break;

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

            case RelationalOperator::NE:
                return "\\ne";
            }
            std::unreachable();
        }

        inline std::string to_html(const RelationalOperator opr) {
            switch (opr) {
            case RelationalOperator::LT:
                return "<mo>&lt;</mo>";

            case RelationalOperator::LE:
                return "<mo>&le;</mo>";

            case RelationalOperator::GT:
                return "<mo>&gt;</mo>";

            case RelationalOperator::GE:
                return "<mo>&ge;</mo>";

            case RelationalOperator::EQ:
                return "<mo>=</mo>";

            case RelationalOperator::NE:
                return "<mo>&ne;</mo>";
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

            case RelationalOperator::EQ:
            case RelationalOperator::NE:
                return opr;
            }
            std::unreachable();
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

    namespace detail {
        template <typename T, typename U, typename V>
        void print_substitute(const T& initial, const std::map<U, Fraction>& values, const V& final) {
            if (!values.empty()) {
                std::string str;

                switch (GLOBAL_FORMATTING.output) {
                case FormatSettings::Output::LATEX:
                    str = "\\left.";
                    str.append(initial.to_latex()).append("\\right|_{");

                    for (const auto& [variable, fraction] : values) {
                        str.append(variable.to_latex()).append("=").append(fraction.to_latex()).push_back(',');
                    }
                    str.pop_back();
                    GLOBAL_FORMATTING << LaTeX(str.append("}=").append(final.to_latex()));

                    break;

                case FormatSettings::Output::HTML:
                    str.append(initial.to_html())
                        .append("<msub><mo>|</mo><mrow>")
                        .append(values.begin()->first.to_html())
                        .append("<mo>=</mo>")
                        .append(values.begin()->second.to_html());

                    for (const auto& [variable, fraction] : values | std::views::drop(1)) {
                        str.append("<mo>,</mo><mspace width='5px'/>").append(variable.to_html()).append("<mo>=</mo>").append(fraction.to_html());
                    }
                    GLOBAL_FORMATTING << HTML(str.append("</mrow></msub><mo>=</mo>").append(final.to_html()));
                    break;

                case FormatSettings::Output::FILE:
                case FormatSettings::Output::CONSOLE:
                    GLOBAL_FORMATTING << initial << "|(" << values.begin()->first << '=' << values.begin()->second;

                    for (const auto& [variable, fraction] : values | std::views::drop(1)) {
                        GLOBAL_FORMATTING << ", " << variable << '=' << fraction;
                    }
                    GLOBAL_FORMATTING << ") = " << final << std::endl;
                }
            }
        }

        template <typename T, typename U, typename V>
        void print_differentiate(const T& initial, const U& wrt, const V& final) {
            switch (GLOBAL_FORMATTING.output) {
            case FormatSettings::Output::LATEX:
                GLOBAL_FORMATTING << LaTeX(
                    std::string("\\dfrac{d}{d").append(wrt.to_latex()).append("}").append(initial.to_latex()).append("=").append(final.to_latex()));
                break;

            case FormatSettings::Output::HTML:
                GLOBAL_FORMATTING << HTML(std::string("<mfrac><mi>d</mi><mrow><mi>d</mi><mi>")
                                              .append(wrt.to_html().append("</mi></mrow></mfrac>"))
                                              .append(initial.to_html())
                                              .append("<mo>=</mo>")
                                              .append(final.to_html()));
                break;

            case FormatSettings::Output::FILE:
            case FormatSettings::Output::CONSOLE:
                GLOBAL_FORMATTING << "d/d" << wrt << "(" << initial << ") = " << final << std::endl;
            }
        }

        template <typename T, typename U, typename V, typename W>
        void print_integrate(const T& initial, const U& a, const U& b, const V& wrt, const W& final) {
            switch (GLOBAL_FORMATTING.output) {
            case FormatSettings::Output::LATEX:
                GLOBAL_FORMATTING << LaTeX(std::string("\\int_{")
                                               .append(a.to_latex())
                                               .append("}^{")
                                               .append(b.to_latex())
                                               .append("}")
                                               .append(initial.to_latex())
                                               .append("d")
                                               .append(wrt.to_latex())
                                               .append("=")
                                               .append(final.to_latex()));
                break;

            case FormatSettings::Output::HTML:
                GLOBAL_FORMATTING << HTML(std::string("<msubsup><mo>&int;</mo>")
                                              .append(a.to_html())
                                              .append(b.to_html())
                                              .append("</msubsup>")
                                              .append(initial.to_html())
                                              .append("<mspace width='5px'/><mi>d</mi>")
                                              .append(wrt.to_html())
                                              .append("<mo>=</mo>")
                                              .append(final.to_html()));
                break;

            case FormatSettings::Output::FILE:
            case FormatSettings::Output::CONSOLE:
                GLOBAL_FORMATTING << "Integration of " << initial << 'd' << wrt << " from " << a << " to " << b << " = " << final << std::endl;
            }
        }
    } // namespace detail
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

        case algebra::RelationalOperator::NE:
            return "!=";
        }
        std::unreachable();
    }
} // namespace std
