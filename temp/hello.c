#include <stdio.h>

void foo(void) {
  printf("I'm foo!\n");
}

int main(void) {
  printf("Hello, world!\n");
  foo();
  return 0;
}
