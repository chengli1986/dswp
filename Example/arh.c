#include <stdio.h>

int main() {
    int res=0;
//    for (long long i = 0; i < 10; i++) {
    for (int i = 0; i < 10; i++) {
        res = 999;
    }
    //printf("return value: %d\n", res);
    // better to have return, better for LLVM IR
    return res;
}
