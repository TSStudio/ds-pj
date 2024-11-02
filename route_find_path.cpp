#include "routes.h"

extern QuadTreeNode* root;
extern std::unordered_map<uint64_t, Node*> nodes;

void init_route_find_path(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/find_path")
    ([](const crow::request& req) {
        auto param = req.get_body_params();
        double lat, lon;
        char* id_start = param.get("start");
        char* id_end = param.get("end");
        if (id_start == nullptr) {
            id_start = req.url_params.get("start");
        }
        if (id_end == nullptr) {
            id_end = req.url_params.get("end");
        }
        if (id_start == nullptr || id_end == nullptr) {
            return crow::response(400);
        }
        uint64_t start = std::stoull(id_start);
        uint64_t end = std::stoull(id_end);
        DijkstraPathFinder dpf(nodes[start], nodes[end], 4);
        dpf.find_path();
        auto path = dpf.get_path();
        auto distance = dpf.get_distance();
        auto travel_time = dpf.get_travel_time(4);
        json j;
        j["distance"] = distance;
        j["travel_time"] = travel_time;
        j["path"] = json::array();
        std::unordered_set<uint64_t> node_ids;
        for (auto e : path) {
            json je;
            je["start"] = e->start->node->id;
            node_ids.insert(e->start->node->id);
            je["end"] = e->end->node->id;
            node_ids.insert(e->end->node->id);
            je["distance"] = e->distance;
            j["path"].push_back(je);
        }
        j["nodes"] = json::object();
        for (auto n : node_ids) {
            json jn;
            jn["id"] = n;
            jn["lat"] = nodes[n]->lat;
            jn["lon"] = nodes[n]->lon;
            j["nodes"][std::to_string(n)] = jn;
        }

        std::string response = j.dump();
        //add header
        auto res = crow::response(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        return res;
    });
}