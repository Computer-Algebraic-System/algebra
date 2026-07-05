#include "algebra.hpp"

using namespace algebra;

int main() {
    // GLOBAL_FORMATTING.toggle_file();
    // GLOBAL_FORMATTING.toggle_latex();
    // GLOBAL_FORMATTING.toggle_svg();
    // GLOBAL_FORMATTING.toggle_html();
    GLOBAL_FORMATTING.toggle_manim();
    detail::FormatSettings& out = GLOBAL_FORMATTING;
    Variable x("x"), y("y"), z("z");
    const Variable v = 2 * (Variable("x") ^ 2) * (Variable("y") ^ 3);
    out << v.substitute({{x, 1}, {y, 2}}) << std::endl;
    out << v << std::endl;
    out << v.differentiate(x) << std::endl;
    out << v.differentiate(y) << std::endl;
    out << v.differentiate(z) << std::endl;
    out << v.integrate(x, 1, 2) << std::endl;
    out << v.integrate(y, 1, 2) << std::endl;
    out << v.integrate(z, 1, 2) << std::endl;
    out << (v + v.differentiate(x) + v.differentiate(y) + v.differentiate(z)) << std::endl;
    out << (v + v.differentiate(x) >= v.differentiate(y) + v.differentiate(z)) << std::endl;
    out << (v + v.differentiate(x) >= v.differentiate(y) + v.differentiate(z)).differentiate(x) << std::endl;
    out << (5 * x + 4 * y) / (7 * z) << std::endl;
    out << (6 * x) / (3 * x) << std::endl;
    out << (8 * x * y) / (4 * x) << std::endl;
    out << (x * y + x * z) / x << std::endl;
    out << (x * x * y) / (x * y) << std::endl;
    out << (3 * x * y + 6 * x * z) / (3 * x) << std::endl;
    out << (x * x - y * y) / (x - y) << std::endl;
    out << (x * x + 2 * x * y + y * y) / (x + y) << std::endl;
    out << (x * y + y * z) / y << std::endl;
    out << (12 * x * y + 18 * x * z) / (6 * x) << std::endl;
    out << (9 * x * x * y) / (3 * x) << std::endl;
    out << (x + y) / (x + z) << std::endl;
    out << (x * y + z) / x << std::endl;
    out << (x + y) / x << std::endl;
    out << (6 * x * x * y + 9 * x * y * y) / (3 * x * y) << std::endl;
    out << (x * y * z) / (x * y * z) << std::endl;
    out << (0 * x + 0 * y) / (5 * z) << std::endl;
    const SimplePolynomial f = (x ^ 2) + z;
    out << f.integrate(x, 2, 4) << std::endl;
    const Definition def({x, y},
                         {
                             {(x ^ 2.7) + 5.5 * y / (y ^ 3.6), "x >= y"},
                             {(y ^ 2.7) + 5.5 * x / (x ^ 3.6), "x < y"},
                             {0, "otherwise"},
                         });
    out << def;
    return 0;
}
