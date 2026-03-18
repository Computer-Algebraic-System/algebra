#include "algebra.hpp"

using namespace algebra;

int main() {
    // GLOBAL_FORMATTING.toggle_file("output.txt");
    // GLOBAL_FORMATTING.toggle_latex("latex.tex");
    detail::FormatSettings& out = GLOBAL_FORMATTING;
    Variable x("x"), y("y"), z("z");
    out << Fraction(2, 3) << std::endl;
    out << Fraction(1, 2) << std::endl;
    const Variable v = 2 * (Variable("x") ^ 2.2) * (Variable("y") ^ 3);
    out << v << std::endl;
    out << v.differentiate(x) << std::endl;
    out << v.differentiate(y) << std::endl;
    out << v.differentiate(z) << std::endl;
    out << (v + v.differentiate(x) + v.differentiate(y) + v.differentiate(z)) << std::endl;
    out << (v + v.differentiate(x) >= v.differentiate(y) + v.differentiate(z)) << std::endl;
    out << (v + v.differentiate(x) >= v.differentiate(y) + v.differentiate(z)).differentiate(x) << std::endl;
    return 0;
}
