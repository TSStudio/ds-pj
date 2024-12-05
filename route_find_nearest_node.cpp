#include "routes.h"

extern QuadTreeNode* root;

void init_route_find_nearest_node(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/find_nearest_node")
    ([](const crow::request& req) {
        auto param = req.get_body_params();
        double lat, lon;
        char* lat_s = param.get("lat");
        char* lon_s = param.get("lon");
        if (lat_s == nullptr) {
            lat_s = req.url_params.get("lat");
        }
        if (lon_s == nullptr) {
            lon_s = req.url_params.get("lon");
        }
        if (lat_s == nullptr || lon_s == nullptr) {
            return crow::response(400);
        }
        lat = std::stod(lat_s);
        lon = std::stod(lon_s);
        NodePtr n = root->find_nearest_node(lat, lon, [](const NodePtr& n) { return n.node->road; });
        json j;
        j["id"] = n.node->id;
        j["lat"] = n.node->lat;
        j["lon"] = n.node->lon;
        j["distance"] = n.distance(lat, lon);
        std::string response = j.dump();
        //add header
        auto res = crow::response(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        return res;
    });
}