#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include "random_polygon.h"
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;


void to_json(json& j, const Vertex& vertex) {
    j = json::array({vertex.x, vertex.y});
}

string polygon_to_json(int n, const vector<Vertex> polygon) {
    json j = {
        {"n", n}, 
        {"vertices", polygon},
    };
    return j.dump();
}



int main() {
    httplib::Server server;
    // server.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
    //     res.set_content("Hello World!", "text/plain");
    // });

    server.Get(R"(/generate/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        auto n = stoi(req.matches[1]);
        printf("n: %d\n", n);
        auto vertices = random_polygon(n);
        auto str = polygon_to_json(n, vertices);
        printf("%s\n", str.c_str());
        // res.set_content(str, "text/plain");
        res.set_content(str, "application/json");
    });

    auto ret = server.set_mount_point("/", "../static");
    if(!ret) {
        printf("Bind static error\n");
    }
    printf("listen start\n");
    server.listen("0.0.0.0", 8080);

    return 0;
}