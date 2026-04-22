#pragma once

class algebra::Complex {
    static constexpr auto serial_class = detail::SerialClass::COMPLEX;

public:
    Fraction real, imag;

    Complex() = default;

    Complex(const double value) : real(value) {}

    Complex(const Fraction& real, const Fraction& imag = 0) : real(real), imag(imag) {}

    template <typename T>
    Complex(const std::complex<T>& complex) : real(complex.real()), imag(complex.imag()) {}

    Complex operator-() const { return {-real, -imag}; }

    Complex& operator+=(const Complex& value) {
        real += value.real;
        imag += value.imag;
        return *this;
    }

    Complex operator+(const Complex& value) const { return Complex(*this) += value; }

    Complex& operator+=(const Fraction& value) { return *this += Complex(value); }

    Complex operator+(const Fraction& value) const { return Complex(*this) += value; }

    Complex& operator-=(const Complex& value) { return *this += -value; }

    Complex operator-(const Complex& value) const { return *this + -value; }

    Complex& operator-=(const Fraction& value) { return *this += -value; }

    Complex operator-(const Fraction& value) const { return *this + -value; }

    Complex& operator*=(const Complex& value) {
        real *= value.real + value.imag;
        imag *= value.real + value.imag;
        return *this;
    }

    Complex operator*(const Complex& value) const { return Complex(*this) *= value; }

    Complex& operator*=(const Fraction& value) { return *this *= Complex(value); }

    Complex operator*(const Fraction& value) const { return Complex(*this) *= value; }

    Complex& operator/=(const Complex& value) {
        const Fraction denominator = (value.real ^ 2) + (value.imag ^ 2);
        assert(denominator != 0);
        real = (real * value.real + imag * value.imag) / denominator;
        imag = (imag * value.real - real * value.imag) / denominator;
        return *this;
    }

    Complex operator/(const Complex& value) const { return Complex(*this) /= value; }

    Complex& operator/=(const Fraction& value) { return *this /= Complex(value); }

    Complex operator/(const Fraction& value) const { return Complex(*this) /= value; }

    // Complex& operator^=(const Complex& value) {
    //     return *this = std::pow(static_cast<std::complex<double>>(*this), static_cast<std::complex<double>>(value));
    // }
    //
    // Complex operator^(const Complex& value) const { return Complex(*this) ^= value; }
    //
    // Complex& operator^=(const Fraction& value) { return *this ^= Complex(value); }
    //
    // Complex operator^(const Fraction& value) const { return Complex(*this) ^= value; }

    bool operator==(const Complex& value) const { return real == value.real && imag == value.imag; }

    template <typename T>
        requires std::integral<T> || std::floating_point<T> || std::is_same_v<T, Fraction>
    explicit operator T() const {
        assert(is_real());
        return static_cast<T>(real);
    }

    template <typename T>
    operator std::complex<T>() const {
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

    std::string to_html() const {
        if (*this == 0) {
            return "<mo>0</mo>";
        }
        std::string res;

        if (real != 0) {
            res.append(real.to_html());

            if (imag != 0) {
                res.append(imag < 0 ? "<mo>-</mo>" : "<mo>+</mo>");
            }
        }
        if (imag != 0) {
            res.append(std::abs(imag).to_html()).append("<mi>i</mi>");
        }
        return res;
    }

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&serial_class), sizeof(serial_class));
        real.serialize(out);
        imag.serialize(out);
    }

    static Complex deserialize(std::ifstream& in) {
        detail::SerialClass type;
        in.read(reinterpret_cast<char*>(&type), sizeof(type));
        assert(type == serial_class);

        Complex res;
        res.real = Fraction::deserialize(in);
        res.imag = Fraction::deserialize(in);
        return res;
    }
};

namespace std {
    inline string to_string(const algebra::Complex& complex) {
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
