#include "fraction.h"
#define GLOBAL_Q 14
#include "Qmathlib.h"

Fraction fraction_from(const int16_t num, const int16_t denom) {
  // This only works because _Q is a macro that doesn't assume type
  return fraction_div(num, denom);
}

Fraction fraction_from_i(const int16_t num) {
  return _Q(num);
}

Fraction fraction_from_f(const float f) {
  return _Q(f);
}

Fraction fraction_add(const Fraction x, const Fraction y) {
  return x + y;
}

Fraction fraction_div(const Fraction x, const Fraction y) {
  return _Qdiv(x, y);
}

Fraction fraction_mult(const Fraction x, const Fraction y) {
  return _Qmpy(x, y);
}

float fraction_to(const Fraction frac) {
  return _QtoF(frac);
}
