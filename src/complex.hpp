#pragma once

class algebra::Complex {
public:
    Fraction real, imag;

    constexpr Complex(const double value) : real(value) {}

    constexpr Complex(const Fraction& real, const Fraction& imag = 0) : real(real), imag(imag) {}

    template <typename T>
    constexpr Complex(const std::complex<T>& complex) : real(complex.real()), imag(complex.imag()) {}

    constexpr Complex operator-() const { return {-real, -imag}; }

    constexpr Complex& operator+=(const Complex& value) {
        real += value.real;
        imag += value.imag;
        return *this;
    }

    constexpr Complex operator+(const Complex& value) const { return Complex(*this) += value; }

    constexpr Complex& operator+=(const Fraction& value) { return *this += Complex(value); }

    constexpr Complex operator+(const Fraction& value) const { return Complex(*this) += value; }

    constexpr Complex& operator-=(const Complex& value) { return *this += -value; }

    constexpr Complex operator-(const Complex& value) const { return *this + -value; }

    constexpr Complex& operator-=(const Fraction& value) { return *this += -value; }

    constexpr Complex operator-(const Fraction& value) const { return *this + -value; }

    constexpr Complex& operator*=(const Complex& value) {
        real *= value.real + value.imag;
        imag *= value.real + value.imag;
        return *this;
    }

    constexpr Complex operator*(const Complex& value) const { return Complex(*this) *= value; }

    constexpr Complex& operator*=(const Fraction& value) { return *this *= Complex(value); }

    constexpr Complex operator*(const Fraction& value) const { return Complex(*this) *= value; }

    constexpr Complex& operator/=(const Complex& value) {
        const Fraction denominator = (value.real ^ 2) + (value.imag ^ 2);
        assert(denominator != 0);
        real = (real * value.real + imag * value.imag) / denominator;
        imag = (imag * value.real - real * value.imag) / denominator;
        return *this;
    }

    constexpr Complex operator/(const Complex& value) const { return Complex(*this) /= value; }

    constexpr Complex& operator/=(const Fraction& value) { return *this /= Complex(value); }

    constexpr Complex operator/(const Fraction& value) const { return Complex(*this) /= value; }

    constexpr Complex& operator^=(const Complex& value) {
        return *this = std::pow(static_cast<std::complex<double>>(*this), static_cast<std::complex<double>>(value));
    }

    constexpr Complex operator^(const Complex& value) const { return Complex(*this) ^= value; }

    constexpr Complex& operator^=(const Fraction& value) { return *this ^= Complex(value); }

    constexpr Complex operator^(const Fraction& value) const { return Complex(*this) ^= value; }

    constexpr bool operator==(const Complex& value) const { return real == value.real && imag == value.imag; }

    template <typename T>
        requires std::integral<T> || std::floating_point<T> || std::is_same_v<T, Fraction>
    constexpr explicit operator T() const {
        assert(is_real());
        return static_cast<T>(real);
    }

    template <typename T>
    constexpr operator std::complex<T>() const {
        return {static_cast<T>(real), static_cast<T>(imag)};
    }

    bool is_real() const { return imag == 0; }

    std::string to_latex() const {
        if (*this == 0) {
            return "0";
        }
        std::string res;

        if (real != 0) {
            res.append(real.to_latex());

            if (imag != 0) {
                res.append(imag < 0 ? " - " : " + ");
            }
        }
        if (imag != 0) {
            res.append(std::abs(imag).to_latex()).push_back('i');
        }
        return res;
    }
};

namespace std {
    string to_string(const algebra::Complex& complex) {
        if (complex == 0) {
            return "0";
        }
        string res;

        if (complex.real != 0) {
            res.append(to_string(complex.real));

            if (complex.imag != 0) {
                res.append(complex.imag < 0 ? " - " : " + ");
            }
        }
        if (complex.imag != 0) {
            res.append(to_string(abs(complex.imag))).push_back('i');
        }
        return res;
    }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const Complex& complex) { return out << std::to_string(complex); }
