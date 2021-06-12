#include "random_polygon.h"

int main() {
    // srand((unsigned)time(NULL));
    auto vertices = random_polygon(50);
    for(auto v: vertices) {
        printf("(%.1f, %.1f)\n", v.x, v.y);
    }
    return 0;
}
