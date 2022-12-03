#include <stdio.h>

int main() {
    printf("Hello, World from process 1");
    for (int i = 0; i < 100000000; ++i) {
        printf("p1 : %d \n", i);
    }
    return 0;
}