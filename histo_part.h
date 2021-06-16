#ifndef HISTO_PART
#define HISTO_PART
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <vector>
#include "json.hpp"

namespace histogram {
using json = nlohmann::json;

struct Point{
    double x,y;
    Point():x(0),y(0) {}
    Point(double x_, double y_):x(x_), y(y_) {}
    Point(const Point& p):x(p.x), y(p.y) {}
};

struct Edge{
    double e_base;
    double e_l, e_r;
    int type;
};


struct Polygon{
    int n;
    int id, fa_id;
    std::vector<Point> a;
    Polygon() {
        a.clear();
        n = 0;
        id = 0;
        fa_id = -1;
    }
};

struct Input {
    Polygon p;
    int base;
};

int in_segment(Point r, Point a, Point b)
{
    double cj;
    cj = (b.x-a.x)*(r.y-a.y) - (b.y-a.y)*(r.x-a.x);
    if (fabs(cj) > 1e-8) return 0;
    double d1, d2, d3;
    d3 = sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
    d1 = sqrt((a.x-r.x)*(a.x-r.x)+(a.y-r.y)*(a.y-r.y));
    d2 = sqrt((b.x-r.x)*(b.x-r.x)+(b.y-r.y)*(b.y-r.y));
    d3 = d3 - d1 - d2;
    if (fabs(d3) < 1e-8) return 1;
    else return 0;
}

Edge check(Edge base_edge, Edge chk_edge)
{
    Edge ret;
    ret.type = chk_edge.type;
    if (base_edge.type == 0 || base_edge.type == 3) {
        if (chk_edge.e_base >= base_edge.e_base) {
            ret.type = -1;
            return ret;
        }
    }
    else {
        if (chk_edge.e_base <= base_edge.e_base) {
            ret.type = -1;
            return ret;
        }
    }
    if (chk_edge.e_l >= base_edge.e_r || chk_edge.e_r <= base_edge.e_l) {
        ret.type = -1;
        return ret;
    }
    ret.e_base = chk_edge.e_base;
    if (base_edge.e_l > chk_edge.e_l) ret.e_l = base_edge.e_l;
    else ret.e_l = chk_edge.e_l;
    if (base_edge.e_r < chk_edge.e_r) ret.e_r = base_edge.e_r;
    else ret.e_r = chk_edge.e_r;
    return ret;
}

std::vector<Polygon> GetBaseHistogram(Polygon pl, int ed_id)
{
    std::vector<Polygon> res;
    Edge pol[5011];
    Edge hist[5011];
    Point gram[5011];
    Point left_pol[5011];
    int base_type;

    res.clear();
    Polygon new_p;
    Edge ths_ed;
    int n;
    Point *a;
    n = pl.n;
    a = new Point[n+2];
    for (int i = 0; i <= n+1; i++) {
        a[i].x = pl.a[i].x;
        a[i].y = pl.a[i].y;
    }
    for (int i = 1; i <= n; i++) {
        if (fabs(a[i].x-a[i+1].x) < 1e-8) {
            pol[i].e_base = a[i].x;
            if (a[i].y < a[i+1].y) {
                pol[i].e_l = a[i].y;
                pol[i].e_r = a[i+1].y;
                pol[i].type = 0; //LEFT
            }
            else {
                pol[i].e_l = a[i+1].y;
                pol[i].e_r = a[i].y;
                pol[i].type = 1; //RIGHT
            }
        }
        else {
            pol[i].e_base = a[i].y;
            if (a[i].x < a[i+1].x) {
                pol[i].e_l = a[i].x;
                pol[i].e_r = a[i+1].x;
                pol[i].type = 2; //UP
            }
            else {
                pol[i].e_l = a[i+1].x;
                pol[i].e_r = a[i].x;
                pol[i].type = 3; //DOWN
            }
        }
    }
    base_type = pol[ed_id].type;
    int tot, ti, rd;
    int ins, cmp_type;
    tot = 0;
    //printf("bt:%d\n", base_type);
    for (int i = 1; i < n; i++) {
        if (base_type == 0 || base_type == 2) {
            ti = ed_id + i;
            if (ti > n) ti = ti - n;
        }
        else {
            ti = ed_id - i;
            if (ti <= 0) ti = ti + n;
        }
        if (pol[ti].type == (base_type^1) || pol[ti].type == base_type) {
            ths_ed = check(pol[ed_id], pol[ti]);
            if (ths_ed.type != -1) {
                ins = 1;
                while (tot > 0) {
                    if (ths_ed.e_r <= hist[tot].e_l) break;
                    if (base_type == 0 || base_type == 3) cmp_type = 1;
                    else cmp_type = 0; 
                    if ((cmp_type == 1 && ths_ed.e_base > hist[tot].e_base)||(cmp_type == 0 && ths_ed.e_base < hist[tot].e_base)) {
                        if (ths_ed.e_r >= hist[tot].e_r) tot--;
                        else {
                            hist[tot].e_l = ths_ed.e_r;
                            break;
                        }
                    }
                    else {
                        if (ths_ed.e_l >= hist[tot].e_l) ins = 0;
                        else {
                            ths_ed.e_r = hist[tot].e_l;
                        }
                        break;
                    }
                }
                if (ins == 1) {
                    tot++;
                    hist[tot].e_base = ths_ed.e_base;
                    hist[tot].e_l = ths_ed.e_l;
                    hist[tot].e_r = ths_ed.e_r;
                }
            }
        }
    }
    int gram_tot;
    gram_tot = 0;
    gram_tot++;
    gram[gram_tot].x = a[ed_id].x; gram[gram_tot].y = a[ed_id].y;
    gram_tot++;
    gram[gram_tot].x = a[ed_id+1].x; gram[gram_tot].y = a[ed_id+1].y;
    for (int i = 1; i <= tot; i++) {
        if (base_type == 0 || base_type == 2) ti = i;
        else ti = tot - i + 1;
        if (base_type == 0) {
            gram_tot++; gram[gram_tot].x = hist[ti].e_base; gram[gram_tot].y = hist[ti].e_r;
            gram_tot++; gram[gram_tot].x = hist[ti].e_base; gram[gram_tot].y = hist[ti].e_l;
        }
        if (base_type == 1) {
            gram_tot++; gram[gram_tot].x = hist[ti].e_base; gram[gram_tot].y = hist[ti].e_l;
            gram_tot++; gram[gram_tot].x = hist[ti].e_base; gram[gram_tot].y = hist[ti].e_r;
        }
        if (base_type == 2) {
            gram_tot++; gram[gram_tot].y = hist[ti].e_base; gram[gram_tot].x = hist[ti].e_r;
            gram_tot++; gram[gram_tot].y = hist[ti].e_base; gram[gram_tot].x = hist[ti].e_l;
        }
        if (base_type == 3) {
            gram_tot++; gram[gram_tot].y = hist[ti].e_base; gram[gram_tot].x = hist[ti].e_l;
            gram_tot++; gram[gram_tot].y = hist[ti].e_base; gram[gram_tot].x = hist[ti].e_r;
        }
    }
    // printf("Histogram:\n");
    // for (int i = 1; i <= gram_tot; i++) {
    //     printf("%.2lf %.2lf\n", gram[i].x, gram[i].y);
    // }
    gram[gram_tot+1].x = gram[1].x;
    gram[gram_tot+1].y = gram[1].y;
    gram[0].x = gram[gram_tot].x;
    gram[0].y = gram[gram_tot].y;
    new_p.a.clear();
    new_p.n = gram_tot;
    for (int i = 0; i <= gram_tot+1; i++) {
        new_p.a.push_back(Point(gram[i].x,gram[i].y));
    }
    res.push_back(new_p);

    int left_tot, bz;
    int last_in, tmp, seg2, nxt, pre;
    bz = 2;
    left_tot = 0;
    last_in = 1;
    ti = ed_id + 1;
    while (1) {
        rd = 0;
        while (1) {
            if (bz > gram_tot) break;
            if (fabs(a[ti].x - gram[bz].x) < 1e-8 && fabs(a[ti].y - gram[bz].y) < 1e-8) {
                nxt = ti+1;
                if (nxt > n) nxt = nxt - n;
                if (fabs(a[nxt].x - gram[bz+1].x) < 1e-8 && fabs(a[nxt].y - gram[bz+1].y) < 1e-8) {
                    ti = nxt+1;
                    if (ti > n) ti = ti - n;
                    bz = bz + 2;
                    rd++;
                    last_in = 1;
                }
                else break;
            }
            else {
                if (rd > 0) {
                    ti = ti - 1;
                    if (ti < 0) ti = ti + n;
                }
                break;
            }
        }
        if (bz > gram_tot) break;
        ti = ti + 1;
        if (ti > n) ti = ti - n;
        pre = ti - 1;
        if (pre < 0) pre = pre + n;
        if (fabs(a[ti].x - gram[bz+1].x) < 1e-8 && fabs(a[ti].y - gram[bz+1].y) < 1e-8) {
            bz = bz + 2;
            tmp = 1;
        }
        else tmp = in_segment(a[ti], gram[bz], gram[bz+1]);
        if (tmp == 0) {
            if (last_in == 1) {
                if (in_segment(gram[bz], a[ti], a[pre]) == 1) {
                    left_tot = 1;
                    left_pol[1].x = gram[bz].x; left_pol[1].y = gram[bz].y;
                    left_tot++;
                    left_pol[left_tot].x = a[ti].x; left_pol[left_tot].y = a[ti].y;
                }
                else {
                    left_tot = 1;
                    left_pol[1].x = a[pre].x; left_pol[1].y = a[pre].y;
                    left_tot++;
                    left_pol[left_tot].x = a[ti].x; left_pol[left_tot].y = a[ti].y;
                }
            }
            else {
                seg2 = in_segment(gram[bz+1], a[ti], a[pre]);
                if (seg2 == 1) {
                    left_tot++;
                    left_pol[left_tot].x = gram[bz+1].x;
                    left_pol[left_tot].y = gram[bz+1].y;
                    // printf("Left Poly:\n");
                    // for (int j = 1; j <= left_tot; j++) {
                    //     printf("%.2lf %.2lf\n", left_pol[j].x, left_pol[j].y);
                    // }
                    new_p.a.clear();
                    new_p.n = left_tot;
                    new_p.a.push_back(Point(left_pol[left_tot].x, left_pol[left_tot].y));
                    for (int j = 1; j <= left_tot; j++) {
                        new_p.a.push_back(Point(left_pol[j].x, left_pol[j].y));
                    }
                    new_p.a.push_back(Point(left_pol[1].x, left_pol[1].y));
                    res.push_back(new_p);
                    left_tot = 0;
                    bz = bz+2;
                    if (in_segment(gram[bz], a[ti], a[pre]) == 1) {
                        if (fabs(a[ti].x - gram[bz].x) > 1e-8 || fabs(a[ti].y - gram[bz].y) > 1e-8) {
                            left_tot++;
                            left_pol[left_tot].x = gram[bz].x; left_pol[left_tot].y = gram[bz].y;
                            left_tot++;
                            left_pol[left_tot].x = a[ti].x, left_pol[left_tot].y = a[ti].y;
                        }
                    }
                }
                else {
                    left_tot++;
                    left_pol[left_tot].x = a[ti].x; left_pol[left_tot].y = a[ti].y;
                }
            }
        }
        else {
            if (last_in == 0) {
                left_tot++;
                left_pol[left_tot].x = a[ti].x;
                left_pol[left_tot].y = a[ti].y;
                // printf("Left Poly:\n");
                // for (int j = 1; j <= left_tot; j++) {
                //     printf("%.2lf %.2lf\n", left_pol[j].x, left_pol[j].y);
                // }
                new_p.a.clear();
                new_p.n = left_tot;
                new_p.a.push_back(Point(left_pol[left_tot].x, left_pol[left_tot].y));
                for (int j = 1; j <= left_tot; j++) {
                    new_p.a.push_back(Point(left_pol[j].x, left_pol[j].y));
                }
                new_p.a.push_back(Point(left_pol[1].x, left_pol[1].y));
                res.push_back(new_p);
                left_tot = 0;
            }
        }
        last_in = tmp;
    }
    delete a;
    return res;
}

std::vector<Polygon> HistoPart(Polygon pl, int ed_id)
{
    int l,r,ted;
    int his_tot;
    std::vector<Polygon> queue, base_res, res;
    his_tot = 0;
    Polygon tmp;
    queue.clear();
    res.clear();
    base_res.clear();
    l = 0;
    r = 0;
    pl.fa_id = -1;
    queue.push_back(pl);
    while (l <= r) {
        if (l == 0) ted = ed_id;
        else ted = queue[l].n;
        base_res = GetBaseHistogram(queue[l], ted);
        tmp = base_res[0];
        tmp.id = his_tot;
        his_tot++;
        tmp.fa_id = queue[l].fa_id;
        res.push_back(tmp);
        for (int i = 1; i < base_res.size(); i++) {
            tmp = base_res[i];
            tmp.fa_id = his_tot - 1;
            r++;
            queue.push_back(tmp);
        }
        l++;
    }
    return res;
}

void to_json(json &j, const Polygon& polygon) {
    j["n"] = polygon.n;
    j["id"] = polygon.id;
    j["parent"] = polygon.fa_id;
    j["vertices"] = json::array();
    auto vertices = polygon.a;

    for(int i = 1; i <= polygon.n; i ++) {
        j["vertices"].push_back({
            vertices[i].x, vertices[i].y
        });
    }
}

void from_json(const json& j, Point& p) {
    j.at(0).get_to(p.x);
    j.at(1).get_to(p.y);
}
void from_json(const json& j, Input& input) {
    j.at("base").get_to(input.base);
    auto& p = input.p;
    j.at("n").get_to(p.n);
    int n = p.n;
    auto vertices = j.at("vertices");
    p.id = 0;
    p.fa_id = -1;
    p.a.resize(n + 2);
    for(int i = 1; i <= n; i ++) {
        vertices.at(i - 1).get_to(p.a[i]);
    }
    p.a[0] = p.a[n];
    p.a[n + 1] = p.a[1];
}
}

#endif