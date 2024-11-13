#include <iostream>
#include "data_loader.h"
#include "crow.h"
#include "routes.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

char** default_filepath;
unsigned int _file_count = 0;

void load_default() {
    _file_count = 2;
    default_filepath = new char*[_file_count];
    default_filepath[0] = new char[100];
    strcpy(default_filepath[0], "data/shanghai.osm");
    default_filepath[1] = new char[100];
    strcpy(default_filepath[1], "data/jiangsu.osm");
    // default_filepath[2] = new char[100];
    // strcpy(default_filepath[2], "data/zhejiang.osm");
    // default_filepath[3] = new char[100];
    // strcpy(default_filepath[3], "data/anhui.osm");
}

int main(int argc, char* argv[]) {
    std::println("[MAIN] Initializing...");
    bool result = false;
    if (argc > 1) {
        std::println("[MAIN] Reading Data...");
        result = data_init_all(argv + 1, argc - 1);
    } else {
        std::println("[MAIN][W] No data file path provided, loading default.");
        load_default();
        result = data_init_all(default_filepath, _file_count);
    }
    if (result) {
        std::println("[MAIN] Initialization successful");
    } else {
        std::println("[MAIN][E] Initialization failed");
        return -1;
    }
    std::println("[MAIN] Starting web server...");
    crow::SimpleApp app;
    init_routes(app);

    app.port(11222).multithreaded().run();

    return 0;
}