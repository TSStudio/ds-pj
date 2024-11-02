#include <iostream>
#include "data_loader.h"
#include "crow.h"
#include "routes.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
extern QuadTreeNode* root;

const char* default_filepath = "data/shanghai.osm";

int main(int argc, char* argv[]) {
    std::cout << "[MAIN] Initializing..." << std::endl;
    bool result = false;
    if (argc > 1) {
        std::cout << "[MAIN] Reading Data: " << argv[1] << std::endl;
        result = data_init(argv[1]);
    } else {
        std::cout << "[MAIN][W] No data file path provided, loading default: " << default_filepath << std::endl;
        result = data_init((char*)default_filepath);
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