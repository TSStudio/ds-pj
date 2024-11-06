#include "routes.h"

extern QuadEdgeTreeNode* root_edge;
extern std::unordered_map<uint64_t, Node*> nodes;

void init_route_get_edges(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/edges")
    ([](const crow::request& req) {
        auto param = req.get_body_params();
        int x, y, z;
        char* x_s = param.get("x");
        char* y_s = param.get("y");
        char* z_s = param.get("z");
        if (x_s == nullptr) {
            x_s = req.url_params.get("x");
        }
        if (y_s == nullptr) {
            y_s = req.url_params.get("y");
        }
        if (z_s == nullptr) {
            z_s = req.url_params.get("z");
        }
        if (x_s == nullptr || y_s == nullptr || z_s == nullptr) {
            return crow::response(400);
        }
        x = std::stoi(x_s);
        y = std::stoi(y_s);
        z = std::stoi(z_s);
        auto _edges = root_edge->get_edges(x, y, z);
        std::string response;
        json j;
        j["nodes"] = json::object();
        j["ways"] = json::object();
        std::unordered_set<uint64_t> node_ids;
        std::unordered_set<uint64_t> ways;
        std::unordered_map<uint64_t, std::vector<EdgePtr>> way_edges;

        for (auto e : _edges) {
            if (e.edge->appear_level_min > z) {
                continue;
            }
            node_ids.insert(e.edge->start->node->id);
            node_ids.insert(e.edge->end->node->id);
            way_edges[e.edge->belong_to_way].push_back(e);
            ways.insert(e.edge->belong_to_way);
        }
        for (auto& w : ways) {
            std::sort(way_edges[w].begin(), way_edges[w].end(), [](EdgePtr a, EdgePtr b) {
                return a.edge->seq < b.edge->seq;
            });
        }
        for (auto w : ways) {
            json jw, jwt;
            jw["id"] = w;
            jw["edges"] = json::array();
            if (way_edges[w].size() == 0) {
                continue;
            }
            for (auto e : way_edges[w]) {
                json je;
                je["start"] = e.edge->start->node->id;
                je["end"] = e.edge->end->node->id;
                je["distance"] = e.edge->distance;
                je["isRoad"] = e.edge->isRoad;
                if (e.edge->isRoad)
                    je["type"] = (std::string) "highway:" + (e.edge->edgetype == nullptr ? "" : e.edge->edgetype);
                else
                    je["type"] = e.edge->edgetype == nullptr ? "" : e.edge->edgetype;
                je["fill"] = e.edge->fill;
                je["way"] = e.edge->belong_to_way;
                je["seq"] = e.edge->seq;
                jw["edges"].push_back(je);
            }
            if (way_edges[w][0].edge->isRoad)
                jw["type"] = (std::string) "highway:" + (way_edges[w][0].edge->edgetype);
            else
                jw["type"] = way_edges[w][0].edge->edgetype == nullptr ? "" : way_edges[w][0].edge->edgetype;
            j["ways"][std::to_string(w)] = jw;
        }

        for (auto n : node_ids) {
            if (nodes.find(n) == nodes.end()) {
                continue;
            }
            json jn;
            jn["id"] = n;
            jn["lat"] = nodes[n]->lat;
            jn["lon"] = nodes[n]->lon;
            j["nodes"][std::to_string(n)] = jn;
        }

        response = j.dump();
        auto res = crow::response(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Content-Type", "application/json");
        return res;
    });
}