#pragma once

class algebra::BigInt {
public:
    boost::multiprecision::cpp_int value;

    BigInt() : value(0) {}
    BigInt(const int value) : value(value) {}

    BigInt operator-() const {
        BigInt res = *this;
        res.value = -res.value;
        return res;
    }

    BigInt operator+(const BigInt& other) const { return BigInt(*this) += other; }

    BigInt& operator+=(const BigInt& other) {
        value += other.value;
        return *this;
    }

    BigInt operator+(const int other) const { return BigInt(*this) += other; }

    BigInt& operator+=(const int& other) {
        value += other;
        return *this;
    }

    BigInt operator-(const BigInt& other) const { return BigInt(*this) -= other; }

    BigInt& operator-=(const BigInt& other) {
        value -= other.value;
        return *this;
    }

    BigInt operator-(const int other) const { return BigInt(*this) -= other; }

    BigInt& operator-=(const int other) {
        value -= other;
        return *this;
    }

    BigInt operator*(const BigInt& other) const { return BigInt(*this) *= other; }

    BigInt& operator*=(const BigInt& other) {
        value *= other.value;
        return *this;
    }

    BigInt operator*(const int other) const { return BigInt(*this) *= other; }

    BigInt& operator*=(const int other) {
        value *= other;
        return *this;
    }

    BigInt operator/(const BigInt& other) const { return BigInt(*this) /= other; }

    BigInt& operator/=(const BigInt& other) {
        value /= other.value;
        return *this;
    }

    BigInt operator/(const int other) const { return BigInt(*this) /= other; }

    BigInt& operator/=(const int other) {
        value /= other;
        return *this;
    }

    BigInt operator%(const BigInt& other) const { return BigInt(*this) %= other; }

    BigInt& operator%=(const BigInt& other) {
        value %= other.value;
        return *this;
    }

    BigInt operator%(const int other) const { return BigInt(*this) %= other; }

    BigInt& operator%=(const int other) {
        value %= other;
        return *this;
    }

    BigInt operator^(const BigInt& other) const {
        assert(other >= 0);
        BigInt base = *this, exp = other, result = 1;

        while (exp > 0) {
            if (exp % 2 != 0) {
                result *= base;
            }
            base *= base;
            exp /= 2;
        }
        return result;
    }

    BigInt& operator^=(const BigInt& other) { return *this = *this ^ other; }

    BigInt operator^(const int other) const { return *this ^ BigInt(other); }

    BigInt& operator^=(const int other) { return *this = *this ^ other; }

    std::strong_ordering operator<=>(const BigInt& other) const {
        return value < other.value ? std::strong_ordering::less : value > other.value ? std::strong_ordering::greater : std::strong_ordering::equal;
    }

    bool operator==(const BigInt& other) const = default;

    template <typename T>
    explicit operator T() const {
        return value.convert_to<T>();
    }

    BigInt nth_root(const BigInt& n) const {
        assert(n > 0);

        if (*this < 0) {
            if (n % 2 == 0) {
                return -1;
            }
            const BigInt r = (-*this).nth_root(n);
            return r == -1 ? -1 : -r;
        }
        if (*this <= 1) {
            return *this;
        }
        BigInt low = 1, high = *this;

        while (low <= high) {
            BigInt mid = (low + high) / 2, p = mid ^ n;

            if (p < *this) {
                low = mid + 1;
            } else if (p > *this) {
                high = mid - 1;
            } else {
                return mid;
            }
        }
        return -1;
    }
};

namespace std {
    inline algebra::BigInt abs(algebra::BigInt x) { return x < 0 ? -x : x; }

    inline algebra::BigInt gcd(algebra::BigInt a, algebra::BigInt b) {
        a = abs(a);
        b = abs(b);

        while (b != 0) {
            const algebra::BigInt temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    inline algebra::BigInt lcm(const algebra::BigInt& a, const algebra::BigInt& b) { return a * b / gcd(a, b); }

    inline string to_string(const algebra::BigInt& big_int) { return big_int.value.convert_to<string>(); }
} // namespace std

inline std::ostream& algebra::operator<<(std::ostream& out, const BigInt& big_int) { return out << std::to_string(big_int); }
