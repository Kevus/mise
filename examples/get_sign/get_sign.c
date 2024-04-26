/*
 * First KLEE tutorial: testing a small function
 */

#include "klee/klee.h"

int get_sign(int x, int y) {
    if (x > 0 && y > 0)
        return 1;
    else if (x < 0 && y < 0)
        return -1;
    else
        return 0;
}

int main() {
  int a, b = 10;
  klee_make_symbolic(&a, sizeof(a), "a");
  klee_make_symbolic(&b, sizeof(b), "b");
  return get_sign(a, b);
} 
