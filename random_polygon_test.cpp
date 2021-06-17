#include "random_polygon.h"

int n;
int main(int argc, char* argv[]) {
    // scanf("%d", &n);
    n = strtol(argv[1], NULL, 10);
    printf("%d\n", n);
    auto output = generation::random_polygon(n, true);
    assert(n == output.polygon.vs.size());
    for(auto v: output.polygon.vs) {
        printf("%.1f %.1f\n", v.x, v.y);
    }
    printf("%d\n", output.polygon.base);
    return 0;
}
