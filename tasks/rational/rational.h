#ifndef RATIONAL_H_
#define RATIONAL_H_


#include <stdexcept>
#include <iostream>
#include <numeric>

class RationalDivisionByZero : public std::runtime_error {
public:
    RationalDivisionByZero() : std::runtime_error("RationalDivisionByZero")
    {
    }
};

class Rational {
public:
    Rational();

    Rational(int value);  // NOLINT

    Rational(int numer, int denom);

    int getNumerator() const;

    int getDenominator() const;

    void setNumerator(int value);

    void setDenominator(int value);

    friend Rational& operator+=(Rational& lhs, const Rational& rhs);

    friend Rational& operator*=(Rational& lhs, const Rational& rhs);

    friend Rational& operator++(Rational& ratio);  // faster than += 1

    friend Rational& operator--(Rational& ratio);

    friend std::istream& operator>>(std::istream& is, Rational& ratio);

private:
    void set(int64_t numer, int64_t denom);

    int numer_;
    int denom_;
};

Rational operator+(const Rational& ratio);

Rational operator-(const Rational& ratio);

Rational& operator-=(Rational& lhs, const Rational& rhs);

Rational& operator/=(Rational& lhs, const Rational& rhs);

Rational operator+(const Rational& lhs, const Rational& rhs);

Rational operator-(const Rational& lhs, const Rational& rhs);

Rational operator*(const Rational& lhs, const Rational& rhs);

Rational operator/(const Rational& lhs, const Rational& rhs);

Rational operator++(Rational& ratio, int);

Rational operator--(Rational& ratio, int);

bool operator<(const Rational& lhs, const Rational& rhs);

bool operator>(const Rational& lhs, const Rational& rhs);

bool operator<=(const Rational& lhs, const Rational& rhs);

bool operator>=(const Rational& lhs, const Rational& rhs);

bool operator==(const Rational& lhs, const Rational& rhs);

bool operator!=(const Rational& lhs, const Rational& rhs);

std::ostream& operator<<(std::ostream& os, const Rational& ratio);


#endif // RATIONAL_H_
