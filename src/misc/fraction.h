#pragma once
#include <stdint.h>

// Abstraction for Qmathlib

typedef int16_t Fraction;

// [-2, 2)
Fraction fraction_from(const int16_t num, const int16_t denom);

// [
Fraction fraction_from_i(const int16_t x);

Fraction fraction_from_f(const float f);

Fraction fraction_add(const Fraction x, const Fraction y);

Fraction fraction_div(const Fraction x, const Fraction y);

// [-2, 2)
Fraction fraction_mult(const Fraction x, const Fraction y);

float fraction_to(const Fraction frac);
