#ifndef RANDOM_POLYGON
#define RANDOM_POLYGON
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <set>
#include <vector>
#include <limits>
#include <random>
#include <unordered_set>
using namespace std;

class RandomGen {
    default_random_engine e;
    uniform_int_distribution<int> u;
public:
    RandomGen(int n): e(time(NULL)) {
        u = uniform_int_distribution<int>(0, n);
    }

    int random() {
        return u(e);
    }
};


template<typename T>
class Node {
public:
    Node<T>* prev;
    Node<T>* next;
    T data; // is the owner of data
    Node(T _data):data(_data), prev(this), next(this) {}

    void link(Node* ele) {
        // build this <-> ele
        this -> next = ele;
        ele -> prev = this;
    }

    void check() {
        assert(this -> prev -> next == this);
        assert(this -> next -> prev == this);
    }

    Node<T>* insert_after(Node* ele) {
        ele -> link(this -> next);
        this -> link(ele);
        check();
        return ele;
    }

    Node<T>* insert_before(Node* ele) {
        return this -> prev -> insert_after(ele);
    }
    
    Node<T>* remove_this() {
        this -> prev -> link(this -> next);
        this -> prev -> check();
        this -> next -> check();
        this -> link(this);
        return this;
    }

    Node<T>* remove_after() {
        return this -> next -> remove_this();
    }

    Node<T>* remove_before() {
        return this -> prev -> remove_this();
    }

    void destory_data() {
        delete data;
    }
};

template<typename T>
class DoublyLinkedList {
public:
    Node<T>* head;
    DoublyLinkedList():head(NULL) {}

    Node<T>* insert(T data) {
        auto new_node = new Node<T>(data);
        if(head) {
            head -> insert_before(new_node);
        } else {
            head = new_node;
        }
        return new_node;
    }

    void destory_data() {
        if(!head) return;
        auto h = head;
        do {
            h -> destory_data();
            h = h -> next;
        } while(h != head);
    }

    void print() {
        auto h = head;
        do {
            h -> data -> print();
            h = h -> next;
        } while(h != head);
    }
    

    ~DoublyLinkedList() {
        if(!head) return;
        while(head -> next != head) {
            auto node = head -> remove_after();
            delete node;
        }
        delete head;
    }
};


class Vertex {
public:
    double x, y;
    Vertex(double _x, double _y):x(_x), y(_y) {}
    Vertex():x(0), y(0) {}
    Vertex(const Vertex& v):x(v.x), y(v.y) {}

    bool in_rectangle(Vertex* v1, Vertex* v2) {
        double x1 = v1 -> x;
        double y1 = v1 -> y;
        double x2 = v2 -> x;
        double y2 = v2 -> y;
        if(y1 > y2) swap(y1, y2);
        if(x1 > x2) swap(x1, x2);
        return x1 <= x && x <= x2 && y1 <= y && y <= y2;
    }

    Vertex operator+ (const Vertex& v) {
        return Vertex(x + v.x, y + v.y);
    }

    Vertex operator- (const Vertex& v) {
        return Vertex(x - v.x, y - v.y);
    }

    void print() {
        printf("(%.1f, %.1f)\n", x, y);
    }
};

class Edge {
public:
    bool vertical;
    Vertex* v1;
    Vertex* v2;
    Edge(Vertex* _v1, Vertex* _v2):v1(_v1), v2(_v2), vertical(_v1 -> x == _v2 -> x) {}

    bool intersect_with_line(int l) {
        // judge whether this intersect with l(th) grid line
        // x/y-range of l(th) grid line is [l, l + 1]
        
        return (vertical && (v1 -> y <= l && v2 -> y >= l + 1) || (v1 -> y >= l + 1 && v2 -> y <= l))
            ||(!vertical && (v1 -> x <= l && v2 -> x >= l + 1) || (v1 -> x >= l + 1 && v2 -> x <= l));
    }

    void print() {
        printf("(%.1f, %.1f) -> (%.1f, %.1f)\n", v1 -> x, v1 -> y, v2 -> x, v2 -> y);
    }
};


struct Entry {
    Vertex* m;
    Node<Edge*>* inter_e;
    bool ccw; // indicate whether
    Entry(Vertex* _m, Node<Edge*>* _inter_e, bool _ccw):m(_m), inter_e(_inter_e), ccw(_ccw) {}
};


void print_edges(const vector<Node<Edge*>*> edges) {
    for(auto e: edges) {
        e -> data -> print();
    }
}

vector<Vertex> random_polygon(int n) {
    int r = n / 2 - 2;
    DoublyLinkedList<Vertex*> vertices;
    // grids[i] (index start from 1)
    // = number of grids between [i, i + 1]
    // total number of grids
    // DoublyLinkedList<int> grids; 
    vector<int> grids;
    int area = 1;
    vector<Node<Edge*>*> vedges;
    vector<Node<Edge*>*> hedges;

    // init sorted edges
    DoublyLinkedList<Edge*> edges;
    auto v1 = new Vertex(1, 1);
    auto v2 = new Vertex(1, 2);
    auto v3 = new Vertex(2, 2);
    auto v4 = new Vertex(2, 1);
    // v1 -- v4 
    // |     |
    // v2 -- v3

    vertices.insert(v1);
    vertices.insert(v2);
    vertices.insert(v3);
    vertices.insert(v4);

    grids.push_back(1);
    auto e12 = edges.insert(new Edge(v1, v2));
    auto e23 = edges.insert(new Edge(v2, v3));
    auto e34 = edges.insert(new Edge(v3, v4));
    auto e41 = edges.insert(new Edge(v4, v1));

    vedges.push_back(e12);
    vedges.push_back(e34);

    hedges.push_back(e41);
    hedges.push_back(e23);

    // printf("\n");
    // printf("horizontal edges: \n");
    // print_edges(hedges);

    // printf("\n");
    // printf("vertical edges: \n");
    // print_edges(vedges);

    while(r > 0) {
        unordered_set<int> is_checked;
        // assert(area <= RAND_MAX);
        // printf("r: %d\n", r);

        auto rg = RandomGen(area - 1);

        while(1) {
            int idx;
            do {
                idx = rg.random();
                // printf("rand..\n");
            } while(is_checked.count(idx) > 0);
            is_checked.insert(idx);
            // printf("idx: %d\n", idx);

            int p, q;
            Node<Edge*>* left = NULL;
            Node<Edge*>* right = NULL;
            Node<Edge*>* top = NULL;
            Node<Edge*>* bottom = NULL;

            // find correct (p, q) of the idx(th) grid
            // find the four lines intersect with the rays started from center of C to NSWE
            // center of C: (p + 1/2, q + 1/2)
            {
                // find q
                int num_of_grids_before_q = 0;
                for(int i = 0; i < grids.size(); i ++) {
                    if(num_of_grids_before_q <= idx && idx < num_of_grids_before_q + grids[i]) {
                        q = i + 1;
                        break;
                    }
                    num_of_grids_before_q += grids[i];
                }

                // sweep vertical edges to find p
                // for sorted vertical edges intersect with line q
                // we could partition them into pairs of [left, right]
                int idx_in_line = idx - num_of_grids_before_q;
                int num_of_grids_before_ve = 0;
                for(auto ve: vedges) {
                    if(!ve -> data -> intersect_with_line(q)) continue;
                    if(!left) {
                        left = ve;
                    } else if (!right) {
                        right = ve;
                        int nl = num_of_grids_before_ve;
                        int nr = nl + right -> data -> v1 -> x - left -> data -> v1 -> x;
                        if(idx_in_line >= nl && idx_in_line < nr) {
                            p = left -> data -> v1 -> x + idx_in_line - nl;
                            break;
                        }
                        num_of_grids_before_ve += right -> data -> v1 -> x - left -> data -> v1 -> x;
                        left = NULL;
                        right = NULL;
                    }
                }

                for(auto he: hedges) {
                    if(!he -> data -> intersect_with_line(p)) continue;
                    if(!top) {
                        top = he;
                    } else if(!bottom) {
                        bottom = he;
                        if(q >= top -> data -> v1 -> y && q + 1 <= bottom -> data -> v1 -> y) {
                            break;
                        }
                        top = NULL;
                        bottom = NULL;
                    }
                }
            }
            // perform cut check

            // printf("(p, q): (%d, %d)\n", p, q);
            // printf("left\n");
            // left -> data -> print();
            // printf("right\n");
            // right -> data -> print();
            // printf("top\n");
            // top -> data -> print();
            // printf("bottom\n");
            // bottom -> data -> print();

            auto c = Vertex(p + 0.5, q + 0.5);
            vector<Entry> ms;
            ms.push_back(Entry(left -> data -> v1, left, false));
            ms.push_back(Entry(left -> data -> v2, left, true));
            ms.push_back(Entry(right -> data -> v1, right, false));
            ms.push_back(Entry(right -> data-> v2, right, true));
            ms.push_back(Entry(top -> data -> v1, top, false));
            ms.push_back(Entry(top -> data -> v2, top, true));
            ms.push_back(Entry(bottom -> data -> v1, bottom, false));
            ms.push_back(Entry(bottom -> data -> v2, bottom, true));

            random_shuffle(ms.begin(), ms.end());
            Entry* en = NULL;
            Node<Vertex*>* m_node = NULL;
            for(auto entry: ms) {
                auto v = vertices.head;
                int cnt = 0;
                do {
                    if(v -> data -> in_rectangle(&c, entry.m)) {
                        m_node = v;
                        cnt ++;
                        if(cnt >= 2) break;
                    }
                    v = v -> next;
                } while(v != vertices.head);
                if(cnt <= 1) {
                    // find appropriate m
                    en = &entry;
                    assert(m_node -> data == en -> m);
                    break;
                }
            }
            if(!en) continue;
            // printf("m\n"); m_node -> data -> print();
            // printf("inter_e\n");en -> inter_e -> data -> print();
            // printf("ccw: %d\n", en -> ccw);

            // inflate
            auto v = vertices.head;
            do {
                if(v -> data -> x > p) {
                    v -> data -> x += 1;
                }
                if(v -> data -> y > q) {
                    v -> data -> y += 1;
                }
                v = v -> next;
            } while(v != vertices.head);
            

            // grids and area ?
            grids.insert(grids.begin() + q - 1, grids[q - 1]);
            area += grids[q - 1];
            Edge* fi = NULL;
            Edge* se = NULL;

            for(auto he: hedges) {
                if(!he -> data -> intersect_with_line(p)) continue;
                if(!fi) {
                    fi = he -> data;
                } else if(!se) {
                    se = he -> data;
                    for(int i = fi -> v1 -> y; i < se -> v1 -> y; i ++) {
                        grids[i - 1] ++;
                    }
                    area += se -> v1 -> y - fi -> v1 -> y;
                    fi = NULL;
                    se = NULL;
                }
            }

            // printf("AFTER INFLATE\n");
            // printf("area: %d\n", area);
            // printf("grid: \n");
            // for(auto g: grids) {
            //     printf("%d ", g);
            // }

            // printf("\n");
            // printf("horizontal edges: \n");
            // print_edges(hedges);

            // printf("\n");
            // printf("vertical edges: \n");
            // print_edges(vedges);

            // printf("\n");
            // printf("Polygon: \n");
            // vertices.print();

            // cut
            Vertex* vc = new Vertex(p + 1, q + 1);
            Node<Edge*>* inter_e = en -> inter_e;
            Vertex* m = en -> m;

            Vertex* s = inter_e -> data -> vertical ? new Vertex(m -> x, vc -> y) : new Vertex(vc -> x, m -> y);
            Vertex* s_bar = new Vertex(*m - *s + *vc);

            Node<Edge*>* s_edge = en -> ccw ? inter_e : inter_e -> prev;
            if(!en -> ccw) swap(s, s_bar);

            // vertices ?
            Node<Vertex*>* prev = m_node -> prev;
            if(m_node == vertices.head) 
                vertices.head = prev;
            prev -> remove_after();
            prev -> insert_after(new Node<Vertex*>(s_bar));
            prev -> insert_after(new Node<Vertex*>(vc));
            prev -> insert_after(new Node<Vertex*>(s));

            // printf("m ");m -> print();
            // printf("c ");vc -> print();
            // printf("s ");s -> print();
            // printf("s_bar "); s_bar -> print();
            // printf("inter_e\n");
            // inter_e -> data -> print();
            // inter_e -> prev -> data -> print();
            // printf("s_edge\n");
            // s_edge -> data -> print();

            // edges ?
            s_edge -> data -> v2 = s;
            s_edge -> next -> data -> v1 = s_bar;
            Node<Edge*>* c_s_bar = s_edge -> insert_after(new Node<Edge*>(new Edge(vc, s_bar)));
            Node<Edge*>* s_c = s_edge -> insert_after(new Node<Edge*>(new Edge(s, vc)));

            // assert(en -> ccw == s_c -> data -> vertical && en -> ccw == !c_s_bar -> data -> vertical);

            // printf("\n");
            // printf("Edges: \n");
            // edges.print();

            // printf("c -> s_bar\n");c_s_bar -> data -> print();
            // printf("s -> c\n");s_c -> data -> print();

            // printf("vertical: %d\n", s_c -> data -> vertical);

            // hedges ?
            // vedges ?
            if(s_c -> data -> vertical) {
                vedges.insert(vedges.begin() + int(s_c -> data -> v1 -> x) - 1, s_c);
                hedges.insert(hedges.begin() + int(c_s_bar -> data -> v1 -> y) - 1, c_s_bar);
            } else {
                hedges.insert(hedges.begin() + int(s_c -> data -> v1 -> y) - 1, s_c);
                vedges.insert(vedges.begin() + int(c_s_bar -> data -> v1 -> x) - 1, c_s_bar);
            }


            // area ?
            int x1 = min(vc -> x, m -> x);
            int x2 = max(vc -> x, m -> x);
            int y1 = min(vc -> y, m -> y);
            int y2 = max(vc -> y, m -> y);
            area -= (y2 - y1) * (x2 - x1);
            for(int i = y1; i < y2; i ++) {
                grids[i - 1] -= x2 - x1;
            }

            // garbage collection
            m_node -> destory_data();
            delete m_node;

            break;
        }
        r --;

        // printf("\n");
        // printf("area: %d\n", area);
        // printf("grid: \n");
        // for(auto g: grids) {
        //     printf("%d ", g);
        // }

        // printf("\n");
        // printf("horizontal edges: \n");
        // print_edges(hedges);

        // printf("\n");
        // printf("vertical edges: \n");
        // print_edges(vedges);

        // printf("\n");
        // printf("Polygon: \n");
        // vertices.print();
    }
    edges.destory_data();
    grids.clear();

    vector<Vertex> vs;
    auto v = vertices.head;
    do {
        vs.push_back(Vertex(*v -> data));
        v = v -> next;
    } while(v != vertices.head);
    vertices.destory_data();
    return vs;
}

#endif