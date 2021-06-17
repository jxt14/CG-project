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
        printf("generate called\n");
        auto n = stoi(req.matches[1]);
        auto output = generation::random_polygon(n, true);
        printf("generate end\n");
        json j = output;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(j.dump(), "application/json");
    });

    server.Post("/partition", [](const httplib::Request& req, httplib::Response& res) {
        printf("partition called\n");
        auto j = json::parse(req.body);
        histogram::Input input;
        j.get_to(input);
        
        json histograms = HistoPart(input.p, input.base);
        auto str = histograms.dump();
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(str, "application/json");
    });

    server.Post("/solve", [](const httplib::Request& req, httplib::Response& res) {
        printf("solve called\n");
        auto j = json::parse(req.body);
        histogram::Input input;
        j.get_to(input);
        input.print();

        json lh = solveLH(input.p, input.base);
        auto str = lh.dump();
        res.set_header("Access-Control-Allow-Origin", "*");
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