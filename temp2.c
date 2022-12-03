#include <stdio.h>

int main() {
    printf("Hello, World from process 2");
    for (int i = 0; i < 100000000; ++i) {
        printf("p2 : %d \n", i);
    }
    return 0;
}