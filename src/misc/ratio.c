#include <misc/ratio.h>

struct Ratio ratio32_mult(const struct Ratio *x, const struct Ratio *y) {
  struct Ratio r = (struct Ratio) {
    .num = x->num * y->num,
    .denom = x->denom
  };

  if ((r.num % y->denom) == 0) {
    r.num /= y->denom;
  } else {
   r.denom *= y->denom;
  }

  return r;
}

float ratio_to(const struct Ratio *r) {
  return (float)r->num / r->denom;
}
