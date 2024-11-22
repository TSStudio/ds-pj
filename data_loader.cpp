#include "data_loader.h"

extern std::unordered_map<uint64_t, Node *> nodes;
extern std::unordered_map<uint64_t, Way *> ways;
extern QuadTreeNode *root;
extern std::unordered_set<ComputedEdge *> computed_edges_individual;

bool data_init_all(char **__filepath, unsigned int _file_count) {
    std::unordered_set<Node *> _transport_stops;
    uint64_t *_node_count = new uint64_t[_file_count];
    uint64_t *_way_count = new uint64_t[_file_count];
    uint64_t *_relation_count = new uint64_t[_file_count];
    pugi::xml_document *__doc = new pugi::xml_document[_file_count];
    pugi::xml_parse_result *__result = new pugi::xml_parse_result[_file_count];
    bool *_meta = new bool[_file_count];
    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        // ---------- FILE READING ----------
        char *_filepath = __filepath[_file_no];
        __result[_file_no] = __doc[_file_no].load_file(_filepath);
        if (!__result[_file_no]) {
            std::println("[DATA_INIT_PHASE1][FILE][E] Error: {} when loading {}", __result[_file_no].description(), _filepath);
            continue;
        }
        std::println("[DATA_INIT_PHASE1][FILE] File {} loaded successfully", _filepath);
    }
    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        char *_filepath = __filepath[_file_no];
        // ---------- META READING ----------
        _meta[_file_no] = false;
        std::ifstream _json_file(_filepath + std::string(".json"));
        if (_json_file.is_open()) {
            _meta[_file_no] = true;
        }
        if (_meta[_file_no]) {
            std::println("[DATA_INIT][META] Meta file loaded successfully");
        } else {
            std::println("[DATA_INIT][META] Meta file not found");
        }
        json __meta = _meta[_file_no] ? json::parse(_json_file) : json();
        if (_meta[_file_no]) {
            _node_count[_file_no] = __meta.at("nodes").get<uint64_t>();
            _way_count[_file_no] = __meta.at("ways").get<uint64_t>();
            _relation_count[_file_no] = __meta.at("relations").get<uint64_t>();
            std::println("[DATA_INIT][META] Meta file parsed successfully");
            std::println("[DATA_INIT][META] {} nodes and {} ways found", _node_count[_file_no], _way_count[_file_no]);
        }
        _json_file.close();
        // ---------- NODE PARSING ----------
        Progress _progress1(_node_count[_file_no]);
        std::println("[DATA_INIT][NODE] Parsing nodes...");
        uint64_t _node_counter = 0;
        for (pugi::xml_node _node : __doc[_file_no].child("osm").children("node")) {
            uint64_t _id = _node.attribute("id").as_ullong();
            double _lat = _node.attribute("lat").as_double();
            double _lon = _node.attribute("lon").as_double();
            char *_name = nullptr;
            for (pugi::xml_node _tag : _node.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "name") == 0) {
                    _name = new char[strlen(_tag.attribute("v").as_string()) + 1];
                    strcpy(_name, _tag.attribute("v").as_string());
                    break;
                }
            }
            Node *_n = new Node(_id, _lat, _lon);
            _n->name = _name;
            nodes[_id] = _n;
            for (pugi::xml_node _child : _node.children("tag")) {
                if (strcmp(_child.attribute("k").as_string(), "railway") == 0 && strcmp(_child.attribute("v").as_string(), "stop") == 0) {
                    _transport_stops.insert(_n);
                    break;
                }
                if (strcmp(_child.attribute("k").as_string(), "highway") == 0 && strcmp(_child.attribute("v").as_string(), "bus_stop") == 0) {
                    _transport_stops.insert(_n);
                    break;
                }
            }
            _node_counter++;
            if (_meta) _progress1.prog(_node_counter);
        }
        _progress1.done();
        std::println("[DATA_INIT][NODE] {} nodes parsed", _node_counter);
        if (!_meta[_file_no]) {
            _node_count[_file_no] = _node_counter;
        }
    }

    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        // ---------- WAY PARSING ----------
        std::println("[DATA_INIT][WAY] Parsing ways...");
        uint64_t _way_counter = 0, _way_err_counter = 0;
        Progress _progress2(_way_count[_file_no]);
        for (pugi::xml_node _way : __doc[_file_no].child("osm").children("way")) {
            Way *_w = new Way(_way.attribute("id").as_ullong());
            //Way *_w = new Way(_way.attribute("id").as_ullong(), "");
            //check if way is a road
            bool _isRoad = false;
            int _appear_level_min = 999;
            std::string _highway;
            char *_highway_ty = nullptr;
            for (pugi::xml_node _tag : _way.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "highway") == 0) {
                    _highway = _tag.attribute("v").as_string();
                    _isRoad = true;
                    _appear_level_min = EdgeUtil::getLevel(_tag.attribute("v").as_string());
                    _highway_ty = new char[strlen(_tag.attribute("v").as_string()) + 1];
                    strcpy(_highway_ty, _tag.attribute("v").as_string());
                    break;
                }
            }
            short _direction = 3;  // &1 = forward, &2 = backward
            for (pugi::xml_node _tag : _way.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "oneway") == 0) {
                    if (strcmp(_tag.attribute("v").as_string(), "yes") == 0) {
                        _direction = 1;
                    } else if (strcmp(_tag.attribute("v").as_string(), "-1") == 0) {
                        _direction = 2;
                    }
                }
            }
            double _speed_limit = 0;
            for (pugi::xml_node _tag : _way.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "maxspeed") == 0) {
                    _speed_limit = std::stod(_tag.attribute("v").as_string()) / 3.6;
                    break;
                }
            }
            char *_name = nullptr;
            for (pugi::xml_node _tag : _way.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "name") == 0) {
                    _name = new char[strlen(_tag.attribute("v").as_string()) + 1];
                    strcpy(_name, _tag.attribute("v").as_string());
                    break;
                }
            }
            if (_speed_limit == 0) {
                _speed_limit = EdgeUtil::getDefaultSpeedLimit(_highway);
            }
            allowance _allow = EdgeUtil::getAllowance(_highway);
            NodePtr _start, _end, _rstart;
            uint64_t _nd_counter = 0;
            for (pugi::xml_node _nd : _way.children("nd")) {
                uint64_t _ref = _nd.attribute("ref").as_ullong();
                if (!nodes.contains(_ref)) {
                    _way_err_counter++;
                    if (_way_err_counter < 3)
                        std::println("[DATA_INIT][WAY][E] Error: Node {} not found", _ref);
                    continue;
                }
                if (_start.node == nullptr) {
                    _start = NodePtr(nodes[_ref]);
                    _rstart = _start;
                } else {
                    _end = NodePtr(nodes[_ref]);
                    Edge *_e = new Edge(_start, _end, _isRoad, _appear_level_min);
                    _e->belong_to_way = _w->id;
                    _e->seq = _nd_counter++;
                    _e->fill = 0;
                    if (_isRoad) {
                        _e->edgetype = _highway_ty;
                    }
                    // } else {
                    //     _e->edgetype = _type_;
                    // }
                    _w->edges.push_back(EdgePtr(_e));
                    _start.node->edges.insert(EdgePtr(_e));
                    _end.node->edges.insert(EdgePtr(_e));
                    _start.node->level = std::min(_start.node->level, _appear_level_min);
                    _end.node->level = std::min(_end.node->level, _appear_level_min);
                    if (_isRoad) {
                        _start.node->road = true;
                        _end.node->road = true;
                        if (_allow.pedestrian) {
                            _start.node->pedestrian = true;
                            _end.node->pedestrian = true;
                        }
                        if (_direction & 1) {
                            ComputedEdge *tmp = new ComputedEdge(_start, _end, _allow, _speed_limit, _name);

                            computed_edges_individual.insert(tmp);
                            _start.node->computed_edges.push_back(tmp);
                            _end.node->computed_edges_in.push_back(tmp);
                        }
                        if (_direction & 2) {
                            ComputedEdge *tmp = new ComputedEdge(_end, _start, _allow, _speed_limit, _name);

                            computed_edges_individual.insert(tmp);
                            _end.node->computed_edges.push_back(tmp);
                            _start.node->computed_edges_in.push_back(tmp);
                        }
                    }
                    _start = _end;
                }
            }
            ways[_w->id] = _w;
            _way_counter++;
            if (_meta) _progress2.prog(_way_counter);
        }
        _progress2.done();
        if (_way_err_counter > 2)
            std::println("[DATA_INIT][WAY][E] ...\n[DATA_INIT][WAY][E] Error: Total of {} nodes not found", _way_err_counter);
        std::println("[DATA_INIT][WAY] {} ways parsed", _way_counter);

        if (!_meta[_file_no]) {
            _way_count[_file_no] = _way_counter;
        }
    }
    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        // ---------- RELATION PARSING ----------
        std::println("[DATA_INIT][RELATION] Parsing relations...");
        uint64_t _relation_counter = 0, _relation_err_counter = 0, _relation_ok_counter = 0;
        Progress _progress3(_relation_count[_file_no]);
        for (pugi::xml_node _relation : __doc[_file_no].child("osm").children("relation")) {
            //check if is route
            bool _isRoute = false;
            int _type = 0;
            for (pugi::xml_node _tag : _relation.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "type") == 0 && strcmp(_tag.attribute("v").as_string(), "route") == 0) {
                    _isRoute = true;
                    break;
                }
            }
            if (!_isRoute) {
                continue;
            }
            std::string _route_type = "";
            for (pugi::xml_node _tag : _relation.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "route") == 0) {
                    _route_type = _tag.attribute("v").as_string();
                    break;
                }
            }
            if (_route_type != "bus" && _route_type != "subway") {
                continue;
            } else {
                _type = _route_type == "bus" ? 8 : 16;
            }
            char *_route_name = nullptr;
            for (pugi::xml_node _tag : _relation.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "name") == 0) {
                    _route_name = new char[strlen(_tag.attribute("v").as_string()) + 1];
                    strcpy(_route_name, _tag.attribute("v").as_string());
                    break;
                }
            }
            uint64_t _id = _relation.attribute("id").as_ullong();

            for (pugi::xml_node _member : _relation.children("member")) {
                if (strcmp(_member.attribute("type").as_string(), "way") == 0) {
                    uint64_t _ref = _member.attribute("ref").as_ullong();
                    if (!ways.contains(_ref)) {
                        _relation_err_counter++;
                        if (_relation_err_counter < 3)
                            std::println("[DATA_INIT][RELATION][E] Error: Way {} not found", _ref);
                        continue;
                    }
                    Way *_w = ways[_ref];
                    NodePtr _start, _end;
                    for (EdgePtr _e : _w->edges) {
                        _start = *(_e.edge->start);
                        _end = *(_e.edge->end);
                        if (_start.node->id == _end.node->id) {
                            continue;
                        }
                        //_start.node->computed_edges.push_back(new ComputedEdge(_start, _end, {false, false, false, _type & 8 ? true : false, _type & 16 ? true : false}, _type & 8 ? 50 : 80, _route_name));
                        _start.node->push_relation(_id, _end.node, {false, false, false, _type & 8 ? true : false, _type & 16 ? true : false, false}, _type & 8 ? 60 : 80, _route_name);
                        _start = _end;
                    }

                    _relation_ok_counter++;
                }
            }
            // if (_route_name != nullptr)
            //     std::println("Processed relation {}", _route_name);
            _relation_counter++;
            if (_meta) _progress3.prog(_relation_counter);
        }
        _progress3.done();
        if (_relation_err_counter > 2)
            std::println("[DATA_INIT][RELATION][E] ...\n[DATA_INIT][RELATION][E] Error: Total of {} ways not found", _relation_err_counter);
        std::println("[DATA_INIT][RELATION] {} ways parsed successfully", _relation_ok_counter);
        std::println("[DATA_INIT][RELATION] {} relations parsed", _relation_counter);

        if (!_meta[_file_no]) {
            _relation_count[_file_no] = _relation_counter;
        }
    }
    std::println("[DATA_INIT][QUADTREE] Inserting nodes into quadtree...");
    Progress _progress3(nodes.size());
    for (auto n : nodes) {
        if (n.second->road)
            root->insert(n.second);
        _progress3.prog_delta(1);
    }
    _progress3.done();
    std::println("[DATA_INIT][META] Writing meta files...");

    for (unsigned int i = 0; i < _file_count; i++) {
        if (!_meta[i] && _node_count[i] > 0 && _way_count[i] > 0) {
            std::ofstream _json_file(__filepath[i] + std::string(".json"));
            json __meta = {
                {"nodes", _node_count[i]},
                {"ways", _way_count[i]},
                {"relations", _relation_count[i]}};
            _json_file << __meta.dump();
            _json_file.close();
        }
    }
    std::ifstream _link_cache_json_file("link_cache.json");
    json _link_cache_json;
    bool iscache = false;
    if (!_link_cache_json_file.is_open()) {
        std::println("[DATA_INIT][META] Link cache file not found, creating new one.");
    } else {
        _link_cache_json = json::parse(_link_cache_json_file);
        //{files:["file1.osm"],links:[{tr:id,rd:id}]}
        //check if file list is the same
        if (_link_cache_json["files"].size() == _file_count) {
            bool _same = true;
            for (unsigned int i = 0; i < _file_count; i++) {
                if (_link_cache_json["files"][i] != __filepath[i]) {
                    _same = false;
                    std::println("[DATA_INIT][META] Link cache file found, but file list is different, creating new one.");
                    break;
                }
            }
            if (_same) {
                iscache = true;
                std::println("[DATA_INIT][META] Link cache file found, using cache.");
            }
        }
    }
    std::println("[DATA_INIT][TRANSPORT_STOPS] Connecting Transport Stops to Nearest Pedestrain.");
    // if (iscache) {
    //     std::println("[DATA_INIT][META] Using link cache.");
    //     Progress _progress4(_link_cache_json["links"].size());
    //     for (auto _link : _link_cache_json["links"]) {
    //         NodePtr _tr = NodePtr(nodes[_link["tr"].get<uint64_t>()]);
    //         NodePtr _rd = NodePtr(nodes[_link["rd"].get<uint64_t>()]);
    //         if (_tr.node != nullptr && _rd.node != nullptr) {
    //             ComputedEdge *tmp1 = new ComputedEdge(_tr, _rd, {false, false, false, false, false, true}, 10, nullptr, 200);
    //             ComputedEdge *tmp2 = new ComputedEdge(_rd, _tr, {false, false, false, false, false, true}, 10, nullptr, 200);
    //             _tr.node->computed_edges.push_back(tmp1);
    //             _tr.node->computed_edges_in.push_back(tmp2);
    //             _rd.node->computed_edges.push_back(tmp2);
    //             _rd.node->computed_edges_in.push_back(tmp1);
    //             computed_edges_individual.insert(tmp1);
    //             computed_edges_individual.insert(tmp2);
    //         }
    //         _progress4.prog_delta(1);
    //     }
    //     _progress4.done();
    // } else {
    //     json _link_cache_json = {
    //         {"files", json::array()},
    //         {"links", json::array()}};
    //     Progress _progress4(_transport_stops.size());
    //     for (auto n : _transport_stops) {
    //         if (n == nullptr) continue;
    //         NodePtr nearest = root->find_nearest_node(n->lat, n->lon, [](const NodePtr &n) { return n.node->pedestrian; });
    //         if (nearest.node != nullptr) {
    //             ComputedEdge *tmp1 = new ComputedEdge(n, nearest, {false, false, false, false, false, true}, 10, nullptr, 200);
    //             ComputedEdge *tmp2 = new ComputedEdge(nearest, n, {false, false, false, false, false, true}, 10, nullptr, 200);
    //             n->computed_edges.push_back(tmp1);
    //             n->computed_edges_in.push_back(tmp2);
    //             nearest.node->computed_edges.push_back(tmp2);
    //             nearest.node->computed_edges_in.push_back(tmp1);
    //             computed_edges_individual.insert(tmp1);
    //             computed_edges_individual.insert(tmp2);
    //             _link_cache_json["links"].push_back({{"tr", n->id}, {"rd", nearest.node->id}});
    //         }
    //         _progress4.prog_delta(1);
    //     }
    //     _progress4.done();
    //     for (unsigned int i = 0; i < _file_count; i++) {
    //         _link_cache_json["files"].push_back(__filepath[i]);
    //     }
    //     std::ofstream _link_cache_json_file("link_cache.json");
    //     _link_cache_json_file << _link_cache_json.dump();
    //     _link_cache_json_file.close();
    //     std::println("[DATA_INIT][META] Link cache file created.");
    // }

    std::println("[DATA_INIT] Processing Reduced Graph...");
    Progress _progress5(computed_edges_individual.size());
    for (auto e : computed_edges_individual) {
        _progress5.prog_delta(1);
        if (e->isInSet) continue;
        ComputedEdgeSet *_ces = new ComputedEdgeSet(e);
        _ces->extend();
        _ces->start->node->computed_edges_set.push_back(_ces);
    }
    _progress5.done();

    delete[] _node_count;
    delete[] _way_count;
    delete[] _meta;
    delete[] __doc;
    delete[] __result;
    return true;
}