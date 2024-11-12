#include <iostream>
#include "data_loader.h"
#include "crow.h"
#include "routes.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

char** default_filepath;
unsigned int _file_count = 0;

void load_default() {
    _file_count = 1;
    default_filepath = new char*[_file_count];
    default_filepath[0] = new char[100];
    strcpy(default_filepath[0], "data/shanghai.osm");
    // default_filepath[1] = new char[100];
    // strcpy(default_filepath[1], "data/jiangsu.osm");
    // default_filepath[2] = new char[100];
    // strcpy(default_filepath[2], "data/zhejiang.osm");
    // default_filepath[3] = new char[100];
    // strcpy(default_filepath[3], "data/anhui.osm");
}

int main(int argc, char* argv[]) {
    std::cout << "[MAIN] Initializing..." << std::endl;
    bool result = false;
    if (argc > 1) {
        std::cout << "[MAIN] Reading Data " << std::endl;
        result = data_init_all(argv + 1, argc - 1);
    } else {
        std::cout << "[MAIN][W] No data file path provided, loading default." << std::endl;
        load_default();
        result = data_init_all(default_filepath, _file_count);
    }
    if (result) {
        std::cout << "[MAIN] Initialization successful" << std::endl;
    } else {
        std::cout << "[MAIN][E] Initialization failed" << std::endl;
        return -1;
    }

    std::cout << "[MAIN] Starting web server..." << std::endl;
    crow::SimpleApp app;
    init_routes(app);

    app.port(11222).multithreaded().run();

    return 0;
}