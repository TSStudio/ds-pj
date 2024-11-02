#ifndef ROUTES_H
#define ROUTES_H

#include "crow.h"
#include "data_loader.h"
#include "nlohmann/json.hpp"
#include <unordered_set>

using json = nlohmann::json;

void init_routes(crow::SimpleApp& app);

#endif