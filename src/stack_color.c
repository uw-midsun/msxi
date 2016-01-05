#include "stack_color.h"

extern unsigned int _stack;
extern unsigned int __STACK_END;

void stack_color() {
  unsigned int *p = &_stack;
  while (p < (&__STACK_END - 3)) {
    *p = 0xF00D;
    p++;
  }
}
