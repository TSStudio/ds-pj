#include "routes.h"

extern QuadTreeNode* root;
extern std::unordered_map<uint64_t, Node*> nodes;

void init_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/")
    ([]() {
        return "Running";
    });
    CROW_ROUTE(app, "/nodes")
    ([](const crow::request& req) {
        auto param = req.get_body_params();
        double lat_min, lat_max, lon_min, lon_max;
        int level;
        char* lat_min_s = param.get("lat_min");
        char* lat_max_s = param.get("lat_max");
        char* lon_min_s = param.get("lon_min");
        char* lon_max_s = param.get("lon_max");
        char* level_s = param.get("level");
        if (lat_min_s == nullptr) {
            lat_min_s = req.url_params.get("lat_min");
        }
        if (lat_max_s == nullptr) {
            lat_max_s = req.url_params.get("lat_max");
        }
        if (lon_min_s == nullptr) {
            lon_min_s = req.url_params.get("lon_min");
        }
        if (lon_max_s == nullptr) {
            lon_max_s = req.url_params.get("lon_max");
        }
        if (level_s == nullptr) {
            level_s = req.url_params.get("level");
        }
        if (lat_min_s == nullptr || lat_max_s == nullptr || lon_min_s == nullptr || lon_max_s == nullptr || level_s == nullptr) {
            return crow::response(400);
        }
        lat_min = std::stod(lat_min_s);
        lat_max = std::stod(lat_max_s);
        lon_min = std::stod(lon_min_s);
        lon_max = std::stod(lon_max_s);
        level = std::stoi(level_s);
        auto _nodes = root->get_nodes(lat_min, lat_max, lon_min, lon_max, level);
        std::string response;
        json j;
        j["nodes"] = json::array();
        std::unordered_set<uint64_t> node_ids;
        for (auto n : _nodes) {
            node_ids.insert(n.node->id);
        }
        for (auto n : _nodes) {
            for (auto e : n.node->edges) {
                if (e.edge->appear_level_min > level) continue;
                if (node_ids.find(e.edge->start->node->id) == node_ids.end()) {
                    node_ids.insert(e.edge->start->node->id);
                }
                if (node_ids.find(e.edge->end->node->id) == node_ids.end()) {
                    node_ids.insert(e.edge->end->node->id);
                }
            }
        }
        std::unordered_set<Edge*> edges;
        for (auto n : node_ids) {
            json jn;
            jn["id"] = n;
            jn["lat"] = nodes[n]->lat;
            jn["lon"] = nodes[n]->lon;
            for (auto e : nodes[n]->edges) {
                if (e.edge->appear_level_min > level) continue;
                // json je;
                // je["start"] = e.edge->start->node->id;
                // je["end"] = e.edge->end->node->id;
                // je["distance"] = e.edge->distance;
                // je["isRoad"] = e.edge->isRoad;
                // jn["edges"].push_back(je);
                edges.insert(e.edge);
            }
            j["nodes"].push_back(jn);
        }
        j["edges"] = json::array();
        for (auto e : edges) {
            json je;
            je["start"] = e->start->node->id;
            je["end"] = e->end->node->id;
            je["distance"] = e->distance;
            je["isRoad"] = e->isRoad;
            j["edges"].push_back(je);
        }

        response = j.dump();
        //add header
        auto res = crow::response(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        return res;

        //return crow::response(response);
    });
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
        NodePtr n = root->find_nearest_node(lat, lon);
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