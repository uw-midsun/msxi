#pragma once
#include <stdint.h>

// Extremely limited ratio code - does not simplify fractions

struct Ratio {
  uint16_t num;
  uint16_t denom;
};

struct Ratio ratio32_add(const struct Ratio *x, const struct Ratio *y);

struct Ratio ratio32_mult(const struct Ratio *x, const struct Ratio *y);

float ratio_to(const struct Ratio *x);
