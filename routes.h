#ifndef ROUTES_H
#define ROUTES_H

#include "crow.h"
#include "data_loader.h"
#include "nlohmann/json.hpp"
#include "algo.h"
#include <unordered_set>

using json = nlohmann::json;

void init_routes(crow::SimpleApp& app);

void init_route_nodes(crow::SimpleApp& app);
void init_route_find_nearest_node(crow::SimpleApp& app);
void init_route_find_path(crow::SimpleApp& app);

#endif