#include "routes.h"

extern QuadTreeNode* root;
extern std::unordered_map<uint64_t, Node*> nodes;

void init_route_salesman(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/salesman")
    ([](const crow::request& req) {
        //check if prefetch request
        if (req.method == crow::HTTPMethod::OPTIONS) {
            auto res = crow::response(200);
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            return res;
        }
        auto errres = crow::response(400);
        errres.add_header("Access-Control-Allow-Origin", "*");
        float heuristic_factor = 1.0;
        auto param = req.get_body_params();
        char* method = param.get("method");
        char* heuristic_factor_str = param.get("heuristic_factor");
        char* key_str = param.get("key");
        char* nodes_str = param.get("nodes");
        if (method == nullptr) {
            method = req.url_params.get("method");
        }
        if (heuristic_factor_str == nullptr) {
            heuristic_factor_str = req.url_params.get("heuristic_factor");
        }
        if (key_str == nullptr) {
            key_str = req.url_params.get("key");
        }
        if (nodes_str == nullptr) {
            nodes_str = req.url_params.get("nodes");
        }
        // Zone z_;
        // if (zoneWbList_str != nullptr) {
        //     std::stod(zoneWbList_str) == 1 ? zoneFilter = WHITELIST : zoneFilter = BLACKLIST;
        //     //param zone is base64 encoded json array [[lat, lon], [lat, lon], ...]
        //     char* zone = param.get("zone");
        //     if (zone == nullptr) {
        //         zone = req.url_params.get("zone");
        //     }
        //     if (zone == nullptr) {
        //         return errres;
        //     }
        //     std::string zone_str = Base64::decode(zone);
        //     json j = json::parse(zone_str);
        //     for (auto& p : j) {
        //         z_.points.push_back({p[0], p[1]});
        //     }
        // }

        std::vector<Node*> _nodes;
        if (nodes_str == nullptr) {
            return errres;
        }
        std::string nodes_str_s = Base64::decode(nodes_str);
        json j = json::parse(nodes_str_s);
        for (auto& n : j) {
            uint64_t id = n[0];
            if (!nodes.contains(id)) {
                return errres;
            }
            _nodes.push_back(nodes[id]);
        }
        if (heuristic_factor_str != nullptr) {
            heuristic_factor = std::stof(heuristic_factor_str);
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
        Salesman::PathFinder pf(_nodes, mtd, key, heuristic_factor);

        Progress p(1);

        auto pres = pf.find_path_no_return();

        //result: [node1_id,node2_id,...] representing the best approach

        p.done_ms();
        json jr = json::array();
        for (auto& r : pres) {
            jr.push_back(r.path_[0]->start->id);
        }

        std::string response = jr.dump();
        //add header
        auto res = crow::response(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        return res;
    });
}