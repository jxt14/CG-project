#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <vector>
#include "histo_part.h"
using namespace histogram;

int main()
{
    int n,ed_id;
    double tx,ty;
    Point *a;
    Polygon p_in;
    std::vector<Polygon> p_res;
    scanf("%d", &n);
    //counterclockwise order
    a = new Point[n+2];
    for (int i = 1; i <= n; i++) {
        scanf("%lf%lf", &tx, &ty);
        a[i].x = tx;
        a[i].y = ty;
    }
    a[n+1].x = a[1].x;
    a[n+1].y = a[1].y;
    a[0].x = a[n].x;
    a[0].y = a[n].y;
    p_in.n = n;
    for (int i = 0; i <= n+1; i++) {
        p_in.a.push_back(Point(a[i].x, a[i].y));
    }
    scanf("%d", &ed_id);
    std::vector<LightHouse> res;
    res = solveLH(p_in, ed_id);
    for (int i = 0; i < res.size(); i++) {
        printf("%.2lf %.2lf %.2lf %.2lf\n", res[i].ax, res[i].ay, res[i].bx, res[i].by);
    }
}