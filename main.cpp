#include "algebra.hpp"

using namespace algebra;

int main() {
    // GLOBAL_FORMATTING.toggle_file("output.txt");
    GLOBAL_FORMATTING.toggle_latex("latex.tex");
    detail::FormatSettings& out = GLOBAL_FORMATTING;
    Variable x("x"), y("y"), z("z");
    out << Fraction(2, 3) << std::endl; // 2/3
    out << Fraction(4, 6) << std::endl; // should simplify → 2/3
    out << Fraction(-4, -6) << std::endl; // → 2/3
    out << Fraction(-4, 6) << std::endl; // → -2/3
    out << Fraction(0, 5) << std::endl; // → 0

    out << (Fraction(1, 2) + Fraction(1, 3)) << std::endl; // 5/6
    out << (Fraction(3, 4) - Fraction(1, 2)) << std::endl; // 1/4
    out << (Fraction(2, 3) * Fraction(3, 4)) << std::endl; // 1/2
    out << (Fraction(2, 3) / Fraction(4, 5)) << std::endl; // 5/6

    out << (Fraction(0, 1) + Fraction(5, 7)) << std::endl; // 5/7
    out << (Fraction(0, 1) * Fraction(5, 7)) << std::endl; // 0

    out << inf << std::endl; // inf
    out << (Fraction(1, 2) + inf) << std::endl; // inf
    out << (inf * Fraction(2, 3)) << std::endl; // inf

    out << (Fraction(2, 3) ^ Fraction(3)) << std::endl; // 8/27
    out << (Fraction(2, 3) ^ Fraction(-2)) << std::endl; // 9/4
    out << (Fraction(-2, 3) ^ Fraction(3)) << std::endl; // -8/27

    out << (Fraction(4, 9) ^ Fraction(1, 2)) << std::endl; // 2/3
    out << (Fraction(8, 27) ^ Fraction(1, 3)) << std::endl; // 2/3
    out << (Fraction(16, 81) ^ Fraction(1, 4)) << std::endl; // 2/3

    out << (Fraction(-8, 1) ^ Fraction(1, 3)) << std::endl; // -2

    out << (Fraction(123456789, 987654321) * Fraction(111111111, 222222222)) << std::endl;
    out << (Fraction(2, 1) ^ Fraction(100)) << std::endl; // huge number

    Fraction a = Fraction(16, 1) ^ Fraction(1, 4); // 2
    out << (a ^ Fraction(4)) << std::endl; // should be 16

    const Variable v = 2 * (Variable("x") ^ 2.2) * (Variable("y") ^ 3);
    out << v.substitute({{x, 1}, {y, 2}});
    out << v << std::endl;
    out << v.differentiate(x) << std::endl;
    out << v.differentiate(y) << std::endl;
    out << v.differentiate(z) << std::endl;
    // out << v.integrate(x, 1, 2) << std::endl;
    // out << v.integrate(y, 1, 2) << std::endl;
    // out << v.integrate(z, 1, 2) << std::endl;
    // out << (v + v.differentiate(x) + v.differentiate(y) + v.differentiate(z)) << std::endl;
    // out << (v + v.differentiate(x) >= v.differentiate(y) + v.differentiate(z)) << std::endl;
    // out << (v + v.differentiate(x) >= v.differentiate(y) + v.differentiate(z)).differentiate(x) << std::endl;

    // out << (5 * x + 4 * y) / (7 * z) << std::endl;
    // out << (6 * x) / (3 * x) << std::endl;
    // out << (8 * x * y) / (4 * x) << std::endl;
    // out << (x * y + x * z) / x << std::endl;
    // out << (x * x * y) / (x * y) << std::endl;
    // out << (3 * x * y + 6 * x * z) / (3 * x) << std::endl;
    // out << (x * x - y * y) / (x - y) << std::endl;
    // out << (x * x + 2 * x * y + y * y) / (x + y) << std::endl;
    // out << (x * y + y * z) / y << std::endl;
    // out << (12 * x * y + 18 * x * z) / (6 * x) << std::endl;
    // out << (9 * x * x * y) / (3 * x) << std::endl;
    // out << (x + y) / (x + z) << std::endl;
    // out << (x * y + z) / x << std::endl;
    // out << (x + y) / x << std::endl;
    // out << (6 * x * x * y + 9 * x * y * y) / (3 * x * y) << std::endl;
    // out << (x * y * z) / (x * y * z) << std::endl;
    // out << (0 * x + 0 * y) / (5 * z) << std::endl;
    // const SimplePolynomial f = (x ^ 2) + z;
    // out << f.integrate(x, 2, 4);
    // const Definition def({x, y},
    //                      {
    //                          {(x ^ 2.7) + 5.5 * y / (y ^ 3.6), "x >= y"},
    //                          {(y ^ 2.7) + 5.5 * x / (x ^ 3.6), "x < y"},
    //                          {Rational(0), "otherwise"},
    //                      });
    // out << def;
    return 0;
}
