#include <cstdint>

#include "rational.h"

Rational::Rational() : numer_(0), denom_(1) {
}

Rational::Rational(int value) : numer_(value), denom_(1) {
}

Rational::Rational(int numer, int denom) {
    set(numer, denom);
}

int Rational::getNumerator() const {
    return numer_;
}

int Rational::getDenominator() const {
    return denom_;
}

void Rational::setNumerator(int value) {
    set(value, denom_);
}

void Rational::setDenominator(int value) {
    set(numer_, value);
}

void Rational::set(int64_t numer, int64_t denom) {
    if (denom == 0) {
        throw RationalDivisionByZero{};
    }
    if (denom < 0) {
        numer = -numer;
        denom = -denom;
    }

    const int64_t gcd = std::gcd(numer, denom);
    numer_ = static_cast<int>(numer / gcd);
    denom_ = static_cast<int>(denom / gcd);
}

Rational& operator+=(Rational& lhs, const Rational& rhs) {
    lhs.set(static_cast<int64_t>(lhs.numer_) * rhs.denom_ +
                static_cast<int64_t>(rhs.numer_) * lhs.denom_,
            static_cast<int64_t>(lhs.denom_) * rhs.denom_);
    return lhs;
}

Rational& operator*=(Rational& lhs, const Rational& rhs) {
    lhs.set(static_cast<int64_t>(lhs.numer_) * rhs.numer_,
            static_cast<int64_t>(lhs.denom_) * rhs.denom_);
    return lhs;
}

Rational& operator++(Rational& ratio) {
    ratio.numer_ += ratio.denom_;
    return ratio;
}

Rational& operator--(Rational& ratio) {
    ratio.numer_ -= ratio.denom_;
    return ratio;
}

std::istream& operator>>(std::istream& is, Rational& ratio) {
    int numer = 0;
    int denom = 1;
    char slash = '\0';
    if (!(is >> numer)) {
        return is;
    }
    if (is.peek() == '/') {
        is >> slash >> denom;
    }
    if (is) {
        ratio.set(numer, denom);
    }
    return is;
}

Rational operator+(const Rational& ratio) {
    return ratio;
}

Rational operator-(const Rational& ratio) {
    return Rational(-ratio.getNumerator(), ratio.getDenominator());
}

Rational& operator-=(Rational& lhs, const Rational& rhs) {
    lhs += -rhs;
    return lhs;
}

Rational& operator/=(Rational& lhs, const Rational& rhs) {
    lhs = Rational(static_cast<int64_t>(lhs.getNumerator()) * rhs.getDenominator(),
                   static_cast<int64_t>(lhs.getDenominator()) * rhs.getNumerator());
    return lhs;
}

Rational operator+(const Rational& lhs, const Rational& rhs) {
    Rational result = lhs;
    result += rhs;
    return result;
}

Rational operator-(const Rational& lhs, const Rational& rhs) {
    Rational result = lhs;
    result -= rhs;
    return result;
}

Rational operator*(const Rational& lhs, const Rational& rhs) {
    Rational result = lhs;
    result *= rhs;
    return result;
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
    Rational result = lhs;
    result /= rhs;
    return result;
}

Rational operator++(Rational& ratio, int) {
    Rational copy = ratio;
    ++ratio;
    return copy;
}

Rational operator--(Rational& ratio, int) {
    Rational copy = ratio;
    --ratio;
    return copy;
}

bool operator<(const Rational& lhs, const Rational& rhs) {
    return static_cast<int64_t>(lhs.getNumerator()) * rhs.getDenominator() <
           static_cast<int64_t>(rhs.getNumerator()) * lhs.getDenominator();
}

bool operator>(const Rational& lhs, const Rational& rhs) {
    return rhs < lhs;
}

bool operator<=(const Rational& lhs, const Rational& rhs) {
    return !(rhs < lhs);
}

bool operator>=(const Rational& lhs, const Rational& rhs) {
    return !(lhs < rhs);
}

bool operator==(const Rational& lhs, const Rational& rhs) {
    return lhs.getNumerator() == rhs.getNumerator() &&
           lhs.getDenominator() == rhs.getDenominator();
}

bool operator!=(const Rational& lhs, const Rational& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Rational& ratio) {
    os << ratio.getNumerator() << '/' << ratio.getDenominator();
    return os;
}
