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
    Edge():e_base(0),e_l(0),e_r(0),type(0) {}
    Edge(double b_, double l_, double r_, int t_): e_base(b_), e_l(l_), e_r(r_), type(t_) {} 
};

bool cmpe(Edge a, Edge b)
{
    return a.e_l > b.e_l;
}

struct MatchEdge{
    double base_l, base_r;
    double e_l, e_r;
    MatchEdge(): base_l(0), base_r(0), e_l(0), e_r(0) {}
    MatchEdge(double bl_, double br_, double l_, double r_):base_l(bl_), base_r(br_), e_l(l_), e_r(r_) {}
};

struct Polygon{
    int n;
    int id, fa_id;
    std::vector<Point> a;
    std::vector<int> son;
    std::vector<Edge> son_base;
    Polygon() {
        a.clear();
        son.clear();
        son_base.clear();
        n = 0;
        id = 0;
        fa_id = -1;
    }
};

struct LightHouse{
    double ax,ay;
    double bx,by;
    LightHouse(): ax(0), ay(0), bx(0), by(0) {}
    LightHouse(double ax_, double ay_, double bx_, double by_): ax(ax_), ay(ay_), bx(bx_), by(by_) {}
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

Edge getEdge(Point a, Point b)
{
    Edge ret;
    if (fabs(a.x-b.x)<1e-8) {
        ret.e_base = a.x;
        if (a.y < b.y) {
            ret.e_l = a.y;
            ret.e_r = b.y;
            ret.type = 0; //LEFT
        }
        else {
            ret.e_l = b.y;
            ret.e_r = a.y;
            ret.type = 1; //RIGHT
        }
    }
    else {
        ret.e_base = a.y;
        if (a.x < b.x) {
            ret.e_l = a.x;
            ret.e_r = b.x;
            ret.type = 2; // UP
        }
        else {
            ret.e_l = b.x;
            ret.e_r = a.x;
            ret.type = 3; //DOWN
        }
    }
    return ret;
}

std::vector<Polygon> GetBaseHistogram(Polygon pl, int ed_id)
{
    std::vector<Polygon> res;
    Edge *pol;
    Edge *hist;
    Point *gram;
    Point *left_pol;
    int base_type;

    res.clear();
    Polygon new_p;
    Edge ths_ed;
    int n, ytot;
    Point *a;
    n = pl.n;
    a = new Point[n+2];
    pol = new Edge[n+4];
    hist = new Edge[n+4];
    gram = new Point[n+4];
    left_pol = new Point[n+4];
    for (int i = 0; i <= n+1; i++) {
        a[i].x = pl.a[i].x;
        a[i].y = pl.a[i].y;
    }
    for (int i = 1; i <= n; i++) {
        pol[i] = getEdge(a[i], a[i+1]);
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
        if (pol[ti].type == (base_type^1)) {
            ths_ed = check(pol[ed_id], pol[ti]);
            if (ths_ed.type != -1) {
                //printf("Insert ed: %.2lf %.2lf %.2lf\n", ths_ed.e_base, ths_ed.e_l, ths_ed.e_r);
                ins = 1;
                ytot = tot;
                while (tot > 0) {
                    if (ths_ed.e_r <= hist[tot].e_l) break;
                    if (ths_ed.e_l >= hist[tot].e_r) {
                        tot--;
                        continue;
                    }
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
                else {
                    tot = ytot;
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
    //printf("GetBaseGram\n");
    for (int i = 0; i <= gram_tot+1; i++) {
        new_p.a.push_back(Point(gram[i].x,gram[i].y));
        //if (i > 0 && i <= gram_tot) {
        //    printf("%.2lf %.2lf\n", gram[i].x, gram[i].y);
        //}
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
                last_in = 1;
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
    delete pol;
    delete left_pol;
    delete hist;
    delete gram;
    return res;
}

std::vector<MatchEdge> GetPropagation(Polygon pl)
{
    std::vector<MatchEdge> ret;
    ret.clear();
    Edge *hist;
    int hist_tot,n;
    hist_tot = 0;
    n = pl.n;
    hist = new Edge[n+3];

    Edge smp_ed, ths_ed, res_ed;
    int base_type;
    
    smp_ed = getEdge(pl.a[2], pl.a[3]);
    base_type = smp_ed.type;
    for (int i = 2; i <= n; i+=2) {
        ths_ed = getEdge(pl.a[i], pl.a[i+1]);
        if (ths_ed.type == base_type) {
            hist_tot++;
            hist[hist_tot] = ths_ed;
        }
        if (ths_ed.type == (base_type^1)) {
            while (hist_tot > 0) {
                if (base_type == 0 || base_type == 2) {
                    if (ths_ed.e_l >= hist[hist_tot].e_r) break;
                    if (ths_ed.e_l <= hist[hist_tot].e_l) {
                        ret.push_back(MatchEdge(hist[hist_tot].e_base, ths_ed.e_base, hist[hist_tot].e_l, hist[hist_tot].e_r));
                        hist_tot--;
                    }
                    else {
                        ret.push_back(MatchEdge(hist[hist_tot].e_base, ths_ed.e_base, ths_ed.e_l, hist[hist_tot].e_r));
                        hist[hist_tot].e_r = ths_ed.e_l;
                    }
                }
                else {
                    if (ths_ed.e_r <= hist[hist_tot].e_l) break;
                    if (ths_ed.e_r <= hist[hist_tot].e_r) {
                        ret.push_back(MatchEdge(hist[hist_tot].e_base, ths_ed.e_base, hist[hist_tot].e_l, hist[hist_tot].e_r));
                        hist_tot--;
                    }
                    else {
                        ret.push_back(MatchEdge(hist[hist_tot].e_base, ths_ed.e_base, hist[hist_tot].e_l, ths_ed.e_r));
                        hist[hist_tot].e_l = ths_ed.e_r;
                    }
                }
            }
        }
    }
    return ret;
}

std::vector<LightHouse> GetLighthouse(Polygon pl)
{
    std::vector<MatchEdge> propa;
    std::vector<Edge> interval;
    Edge *tmp_int;
    interval.clear();
    propa = GetPropagation(pl);
    Edge my_base, s_base;
    my_base = getEdge(pl.a[1], pl.a[2]);
    double s_c;
    double tmpl;
    int tot, num_son;
    tot = 0;
    num_son = pl.son_base.size();
    tmp_int = new Edge[num_son + 2];

    for (int i = 0; i < propa.size(); i++) {
        if (propa[i].base_l > propa[i].base_r) {
            tmpl = propa[i].base_l;
            propa[i].base_l = propa[i].base_r;
            propa[i].base_r = tmpl;
        }
    }

    int tc;
    for (int i = 0; i < num_son; i++) {
        s_base = pl.son_base[i];
        tc = 0;
        if (fabs(s_base.e_l - my_base.e_base) > fabs(s_base.e_r - my_base.e_base)) s_c = s_base.e_l;
        else s_c = s_base.e_r;
        for (int j = 0; j < propa.size(); j++) {
            if (propa[j].e_l <= s_c && propa[j].e_r >= s_c) {
                if (fabs(propa[j].base_l - s_base.e_base) < 1e-8 || fabs(propa[j].base_r - s_base.e_base) < 1e-8) {
                    tot++;
                    tc = 1;
                    tmp_int[tot] = Edge(my_base.e_base, propa[j].base_l, propa[j].base_r, my_base.type);
                }
            }
            if (tc == 1) break;
        }
    }

    std::sort(tmp_int+1, tmp_int+1+tot, cmpe);
    double minl;
    for (int i = 1; i <= tot; i++) {
        if (i == 1) {
            minl = tmp_int[i].e_l;
            interval.push_back(tmp_int[i]);
        }
        else {
            if (tmp_int[i].e_r < minl) {
                minl = tmp_int[i].e_l;
                interval.push_back(tmp_int[i]);
            }
        }
    }

    std::vector<LightHouse> ret;
    Edge ths_ed;
    ret.clear();
    int ti, bz;
    bz = 0;
    for (int i = 3; i <= pl.n; i+=2) {
        if (my_base.type == 0 || my_base.type == 2) ti = i;
        else ti = pl.n + 2 - i;
        ths_ed = getEdge(pl.a[ti], pl.a[ti+1]);
        while (bz < interval.size()) {
            if (ths_ed.e_l <= interval[bz].e_l && ths_ed.e_r >= interval[bz].e_l) {
                if (my_base.type == 0 || my_base.type == 1) {
                    ret.push_back(LightHouse(ths_ed.e_base, interval[bz].e_l, my_base.e_base, interval[bz].e_l));
                }
                else {
                    ret.push_back(LightHouse(interval[bz].e_l, ths_ed.e_base, interval[bz].e_l, my_base.e_base));
                }
                bz++;
            }
            else break;
        }
        if (bz >= interval.size()) break;
    }
    delete tmp_int;
    return ret;

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

    int fa;
    Edge s_base;
    //solve the lighthouse problem
    for (int i = 0; i < res.size(); i++) {
        fa = res[i].fa_id;
        s_base = getEdge(res[i].a[1], res[i].a[2]);
        if (fa != -1) {
            res[fa].son.push_back(i);
            res[fa].son_base.push_back(s_base);
        }
    }

    return res;
}

std::vector<LightHouse> solveLH(Polygon pl, int ed_id)
{
    std::vector<Polygon> hists;
    std::vector<LightHouse> ret, tmplh;

    hists = HistoPart(pl, ed_id);

    ret.clear();
    for (int i = 0; i < hists.size(); i++) {
        if (hists[i].son.size() != 0) {
            tmplh = GetLighthouse(hists[i]);
            for (int j = 0; j < tmplh.size(); j++) ret.push_back(tmplh[j]);
        }
    }

    ret.push_back(LightHouse(pl.a[ed_id].x, pl.a[ed_id].y, pl.a[ed_id+1].x, pl.a[ed_id+1].y));
    return ret;

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