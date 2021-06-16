#include "random_polygon.h"

int n;
int main(int argc, char* argv[]) {
    // scanf("%d", &n);
    n = strtol(argv[1], NULL, 10);
    printf("%d\n", n);
    auto input = generation::random_polygon(n);
    assert(n == input.vs.size());
    for(auto v: input.vs) {
        printf("%.1f %.1f\n", v.x, v.y);
    }
    printf("%d\n", input.base);
    return 0;
}
