#include <stdio.h>
#include <stdlib.h>
#include "simple_sync.h"

void *test_delegate(void *fake_args) {
  unsigned long long *args = (unsigned long long *)fake_args;
  printf("arg0: %lld\n", args[0]);
  printf("arg1: %lld\n", args[1]);
  printf("arg2: %lld\n", args[2]);
  return NULL;
}

int main() {
  /* test two threads delegate */
  unsigned long long args_1[] = {42, 43, 44};
  unsigned long long args_2[] = {97, 98, 99};
  sync_delegate(0, test_delegate, args_1);
  sync_delegate(1, test_delegate, args_2);
  sync_join();
  return 0;
}
