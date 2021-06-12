#include <cstdio>
#include <cstdlib>
#include <algorithm>

struct Point{
    long long x,y;
};

Point a[10011];

int n;

int main()
{
    int tx,ty;
    scanf("%d", &n);
    for (int i = 1; i <= n; i++) {
        scanf("%d%d", &tx, &ty);
        a[i].x = tx;
        a[i].y = ty;
    }
    

}