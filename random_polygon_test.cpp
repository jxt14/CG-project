#include "random_polygon.h"

int n;
int main() {
    scanf("%d", &n);
    printf("%d\n", n);
    auto input = generation::random_polygon(n);
    assert(n == input.vs.size());
    for(auto v: input.vs) {
        printf("%.1f, %.1f\n", v.x, v.y);
    }
    printf("%d\n", input.base);
    return 0;
}
