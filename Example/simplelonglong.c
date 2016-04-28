#include <stdio.h>

long long fun(long long a) {
	int i;
        long long res;
	res = 0;
	for (i = 0; i < a; i++) {
	        long long x = i;
		res = res + x;
	}
	return res;
}

int main() {
    long long res;
    res = fun(1000000000);
    printf("return value: %lld\n", res);
    return 0;
}
