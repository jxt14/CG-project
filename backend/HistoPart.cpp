#include <cstdio>
#include <cstdlib>
#include <algorithm>

struct Point{
    double x,y;
};

Point a[10011];

int n;
int ed_id;

int main()
{
    double tx,ty;
    scanf("%d", &n);
    for (int i = 1; i <= n; i++) {
        scanf("%lf%lf", &tx, &ty);
        a[i].x = tx;
        a[i].y = ty;
    }
    scanf("%d", &ed_id);
    

}