#include <stdio.h>

double fun(double a) {
	int i;
	double res;
	res = 0;
	for (i = 0; i < a; i++) {
		double x = i;
		res = res + x;
	}
	return res;
}

int main() {
  double res;
  res = fun(10000);
  printf("return value: %f\n", res);
  return 0;
}
