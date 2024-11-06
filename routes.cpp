#include "routes.h"

extern QuadTreeNode* root;
extern std::unordered_map<uint64_t, Node*> nodes;

void init_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/")
    ([]() {
        return "Running";
    });
    init_route_nodes(app);
    init_route_find_nearest_node(app);
    init_route_find_path(app);
    init_route_get_edges(app);
}