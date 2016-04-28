#include <stdio.h>

int fun(int a) {
	int i;
	int res;
	res = 0;
	for (i = 0; i < a; i++) {
	    int x = i;
	    res = res + x;
	}
	return res;
}

int main() {
  int res;
  res = fun(10000);
  printf("return value: %d\n", res);
  return 0;
}
