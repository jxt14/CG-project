#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include "random_polygon.h"
#include "histo_part.h"
#include <cassert>
#ifdef _WIN32
#include "httplib.h"
#include <Windows.h>
#else
#include "httplib.h"
#endif
#include "json.hpp"

using json = nlohmann::json;

int main() {
    httplib::Server server;

    server.Get(R"(/generate/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        auto n = stoi(req.matches[1]);
        printf("n: %d\n", n);
        auto input = generation::random_polygon(n);
        auto str = generation::polygon_to_json(n, input.vs, input.base);
        printf("%s\n", str.c_str());
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(str, "application/json");
    });

    server.Post("/partition", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        auto j = json::parse(req.body);
        histogram::Input input;
        j.get_to(input);
        
        json histograms = HistoPart(input.p, input.base);
        auto str = histograms.dump();
        printf("%s\n", str.c_str());
        res.set_content(str, "application/json");
    });


    auto ret = server.set_mount_point("/", "../static");
    if (!ret) {
        printf("Bind static error\n");
    }
    printf("listen start\n");
    server.listen("0.0.0.0", 8080);

    return 0;
}