#pragma once

class algebra::Point {
    static constexpr auto serial_class = detail::SerialClass::POINT;

public:
    Fraction x, y;

    std::strong_ordering operator<=>(const Point&) const = default;

    std::string to_latex() const { return std::string("\\left(").append(x.to_latex()).append(",").append(y.to_latex()).append("\\right)"); }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        x.serialize(out);
        y.serialize(out);
    }

    static Point deserialize(std::ifstream& in) {
        detail::SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        Point res;
        res.x = Fraction::deserialize(in);
        res.y = Fraction::deserialize(in);
        return res;
    }
};

namespace std {
    inline string to_string(const algebra::Point& point) {
        return string("(").append(to_string(point.x)).append(", ").append(to_string(point.y)).append(")");
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Point& point) { return out << std::to_string(point); }

class algebra::Graph {
    inline static std::string interpreter_path = "/home/dream/.virtualenvs/python/bin/python";

    static pybind11::object& get_plot_fn() {
        static auto guard = [] -> pybind11::scoped_interpreter {
            PyConfig config;
            PyConfig_InitPythonConfig(&config);
            PyConfig_SetString(&config, &config.program_name, Py_DecodeLocale(interpreter_path.c_str(), nullptr));
            pybind11::scoped_interpreter interpret{&config};
            PyConfig_Clear(&config);
            return interpret;
        }();
        static auto plot_fn = [] -> pybind11::object {
            pybind11::module_::import("sys").attr("path").attr("insert")(
                0, (std::filesystem::path(__FILE__).parent_path().parent_path() / "utils").string());
            return pybind11::module_::import("graph").attr("plot");
        }();
        return plot_fn;
    }

public:
    static int plot(const std::vector<Inequation>& inequations, const std::vector<Point>& points = {}, const Fraction& limit = 10,
                    const std::string& file_name = "graph.png") {
        const Fraction increment = limit / 100;
        std::vector<double> x_vals;
        std::vector<std::pair<std::vector<double>, std::string>> y_curves;
        std::vector<std::pair<double, std::string>> vertical_lines;
        std::vector<std::pair<std::string, std::string>> pt_list;
        std::map<Variable, Fraction> substituent{{Variable("x"), Fraction(0)}};

        for (const auto& inequation : inequations) {
            if (std::ranges::contains(std::array{inequation.lhs.terms, inequation.rhs.terms} | std::views::join, Variable("y"), &Variable::basis)) {
                const SimplePolynomial simplified = inequation.solve_for(Variable("y")).rhs;
                std::vector<double> y_vals;

                for (Fraction i = 0; i < limit; i += increment) {
                    substituent.begin()->second = i;
                    y_vals.push_back(static_cast<double>(static_cast<Fraction>(simplified.substitute(substituent, false))));
                }
                y_curves.emplace_back(std::move(y_vals), std::to_string(inequation));
            } else {
                vertical_lines.emplace_back(static_cast<double>(static_cast<Fraction>(inequation.solve_for(Variable("x")).rhs)),
                                            std::to_string(inequation));
            }
        }
        for (Fraction i = 0; i < limit; i += increment) {
            x_vals.push_back(static_cast<double>(i));
        }
        for (const auto& [px, py] : points) {
            pt_list.emplace_back(std::to_string(px), std::to_string(py));
        }
        const int code = get_plot_fn()(file_name, x_vals, y_curves, pt_list, vertical_lines).cast<int>();

        if (GLOBAL_FORMATTING.output == detail::FormatSettings::Output::LATEX) {
            std::string latex("\\begin{center}\n\\includegraphics[width=0.6\\textwidth]{");
            latex.append(file_name).append("}\n\\end{center}\n");
            GLOBAL_FORMATTING << latex;
        }
        return code;
    }
};
