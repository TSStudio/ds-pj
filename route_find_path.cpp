#include "routes.h"

extern QuadTreeNode* root;
extern std::unordered_map<uint64_t, Node*> nodes;

void init_route_find_path(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/find_path")
    ([](const crow::request& req) {
        bool heuristic = false;
        bool view_search_range = false;
        double heuristic_factor = 1.0;
        auto param = req.get_body_params();
        char* id_start = param.get("start");
        char* id_end = param.get("end");
        char* method = param.get("method");
        char* heuristic_factor_str = param.get("heuristic_factor");
        char* view_search_range_str = param.get("view_search_range");
        char* key_str = param.get("key");
        if (id_start == nullptr) {
            id_start = req.url_params.get("start");
        }
        if (id_end == nullptr) {
            id_end = req.url_params.get("end");
        }
        if (method == nullptr) {
            method = req.url_params.get("method");
        }
        if (heuristic_factor_str == nullptr) {
            heuristic_factor_str = req.url_params.get("heuristic_factor");
        }
        if (view_search_range_str == nullptr) {
            view_search_range_str = req.url_params.get("view_search_range");
        }
        if (key_str == nullptr) {
            key_str = req.url_params.get("key");
        }
        if (heuristic_factor_str != nullptr) {
            heuristic = true;
            heuristic_factor = std::stod(heuristic_factor_str);
        }
        if (view_search_range_str != nullptr) {
            view_search_range = true;
        }
        if (id_start == nullptr || id_end == nullptr) {
            return crow::response(400);
        }
        int mtd = 0;
        if (method == nullptr) {
            mtd = 15;
        } else {
            mtd = std::stoi(method);
        }
        int key = 0;
        if (key_str != nullptr) {
            key = std::stoi(key_str);
        }
        uint64_t start = std::stoull(id_start);
        uint64_t end = std::stoull(id_end);
        DijkstraPathFinder* dpf;
        if (heuristic) {
            dpf = new HeuristicOptimizedDijkstraPathFinder(nodes[start], nodes[end], mtd, key, heuristic_factor);
        } else {
            dpf = new DijkstraPathFinder(nodes[start], nodes[end], mtd, key);
        }
        // DijkstraPathFinder dpf(nodes[start], nodes[end], mtd);
        // dpf.find_path();
        Progress p(1);
        dpf->find_path();
        p.done_ms();
        auto path = dpf->get_path();
        auto distance = dpf->get_distance();
        auto travel_time = dpf->get_travel_time();
        json j;
        j["distance"] = distance;
        j["travel_time"] = travel_time;
        j["path"] = json::array();
        j["heuristic_factor"] = heuristic_factor;
        std::unordered_set<uint64_t> node_ids;
        for (auto e : path) {
            json je;
            je["start"] = e->start->node->id;
            node_ids.insert(e->start->node->id);
            je["end"] = e->end->node->id;
            node_ids.insert(e->end->node->id);
            je["distance"] = e->distance;
            je["method"] = e->method;
            if (e->name != nullptr) {
                je["name"] = e->name;
            }
            j["path"].push_back(je);
        }

        if (view_search_range) {
            j["nodes_ch"] = json::array();
            //put all nodes searched instead of only nodes in path
            auto ns = dpf->get_convex_hull_of_visited_nodes();
            for (auto n : ns) {
                json jn;
                jn["id"] = n->id;
                jn["lat"] = n->lat;
                jn["lon"] = n->lon;
                j["nodes_ch"].push_back(jn);
            }
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
        delete dpf;
        return res;
    });
}