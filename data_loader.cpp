#include "data_loader.h"

extern std::unordered_map<uint64_t, Node *> nodes;
extern std::unordered_map<uint64_t, Way *> ways;

QuadTreeNode *root = new QuadTreeNode(-90, 90, -180, 180);

bool data_init_all(char **__filepath, unsigned int _file_count) {
    std::unordered_set<Node *> _railway_stops;
    uint64_t *_node_count = new uint64_t[_file_count];
    uint64_t *_way_count = new uint64_t[_file_count];
    uint64_t *_relation_count = new uint64_t[_file_count];
    bool *_meta = new bool[_file_count];
    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        // ---------- FILE READING ----------
        char *_filepath = __filepath[_file_no];
        pugi::xml_document _doc;
        pugi::xml_parse_result _result = _doc.load_file(_filepath);
        if (!_result) {
            std::cout << "[DATA_INIT_PHASE1][FILE][E] Error: " << _result.description() << " when loading " << _filepath << std::endl;
            continue;
        }
        std::cout
            << "[DATA_INIT_PHASE1][FILE] File " << _filepath << " loaded successfully" << std::endl;
        // ---------- META READING ----------
        _meta[_file_no] = false;
        std::ifstream _json_file(_filepath + std::string(".json"));
        if (_json_file.is_open()) {
            _meta[_file_no] = true;
        }
        if (_meta[_file_no]) {
            std::cout << "[DATA_INIT][META] Meta file loaded successfully" << std::endl;
        } else {
            std::cout << "[DATA_INIT][META][W] Meta file not found" << std::endl;
        }
        json __meta = _meta[_file_no] ? json::parse(_json_file) : json();
        if (_meta[_file_no]) {
            _node_count[_file_no] = __meta.at("nodes").get<uint64_t>();
            _way_count[_file_no] = __meta.at("ways").get<uint64_t>();
            _relation_count[_file_no] = __meta.at("relations").get<uint64_t>();
            std::cout << "[DATA_INIT][META] Meta file parsed successfully" << std::endl;
            std::cout << "[DATA_INIT][META] " << _node_count[_file_no] << " nodes and " << _way_count[_file_no] << " ways found" << std::endl;
        }
        _json_file.close();
        // ---------- NODE PARSING ----------
        Progress _progress1(_node_count[_file_no]);
        std::cout << "[DATA_INIT][NODE] Parsing nodes..." << std::endl;
        uint64_t _node_counter = 0;
        for (pugi::xml_node _node : _doc.child("osm").children("node")) {
            uint64_t _id = _node.attribute("id").as_ullong();
            double _lat = _node.attribute("lat").as_double();
            double _lon = _node.attribute("lon").as_double();
            Node *_n = new Node(_id, _lat, _lon);
            nodes[_id] = _n;
            for (pugi::xml_node _child : _node.children("tag")) {
                if (strcmp(_child.attribute("k").as_string(), "railway") == 0 && strcmp(_child.attribute("v").as_string(), "stop") == 0) {
                    _railway_stops.insert(_n);
                    break;
                }
            }
            _node_counter++;
            if (_meta) _progress1.prog(_node_counter);
        }
        _progress1.done();
        std::cout << "[DATA_INIT][NODE] " << _node_counter << " nodes parsed successfully" << std::endl;

        if (!_meta[_file_no]) {
            _node_count[_file_no] = _node_counter;
        }
    }

    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        // ---------- FILE READING ----------
        char *_filepath = __filepath[_file_no];
        pugi::xml_document _doc;
        pugi::xml_parse_result _result = _doc.load_file(_filepath);
        if (!_result) {
            std::cout << "[DATA_INIT_PHASE2][FILE][E] Error: " << _result.description() << " when loading " << _filepath << std::endl;
            continue;
        }
        std::cout
            << "[DATA_INIT_PHASE2][FILE] File " << _filepath << " loaded successfully" << std::endl;
        // ---------- WAY PARSING ----------
        std::cout << "[DATA_INIT][WAY] Parsing ways..." << std::endl;
        uint64_t _way_counter = 0, _way_err_counter = 0;
        Progress _progress2(_way_count[_file_no]);
        for (pugi::xml_node _way : _doc.child("osm").children("way")) {
            Way *_w = new Way(_way.attribute("id").as_ullong());
            //Way *_w = new Way(_way.attribute("id").as_ullong(), "");
            //check if way is a road
            bool _isRoad = false;
            int _appear_level_min = 999;
            std::string _highway;
            for (pugi::xml_node _tag : _way.children("tag")) {
                if (strcmp(_tag.attribute("k").as_string(), "highway") == 0) {
                    _highway = _tag.attribute("v").as_string();
                    _isRoad = true;
                    _appear_level_min = EdgeUtil::getLevel(_tag.attribute("v").as_string());
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
            NodePtr _start, _end;
            for (pugi::xml_node _nd : _way.children("nd")) {
                uint64_t _ref = _nd.attribute("ref").as_ullong();
                if (nodes.find(_ref) == nodes.end()) {
                    _way_err_counter++;
                    if (_way_err_counter < 3)
                        std::cout << "[DATA_INIT][WAY][E] Error: Node " << _ref << " not found" << std::endl;
                    continue;
                }
                if (_start.node == nullptr) {
                    _start = NodePtr(nodes[_ref]);
                } else {
                    _end = NodePtr(nodes[_ref]);
                    Edge *_e = new Edge(_start, _end, _isRoad, _appear_level_min);
                    _w->edges.push_back(EdgePtr(_e));
                    _start.node->edges.insert(EdgePtr(_e));
                    _end.node->edges.insert(EdgePtr(_e));
                    _start.node->level = std::min(_start.node->level, _appear_level_min);
                    _end.node->level = std::min(_end.node->level, _appear_level_min);
                    if (_isRoad) {
                        _start.node->road = true;
                        if (_allow.pedestrian) {
                            _start.node->pedestrian = true;
                            _end.node->pedestrian = true;
                        }
                        if (_direction & 1) {
                            _start.node->computed_edges.push_back(new ComputedEdge(_start, _end, _allow, _speed_limit, _name));
                        }
                        if (_direction & 2) {
                            _end.node->computed_edges.push_back(new ComputedEdge(_end, _start, _allow, _speed_limit, _name));
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
            std::cout << "[DATA_INIT][WAY][E] ...\n"
                      << "[DATA_INIT][WAY][E] Error: Total of " << _way_err_counter << " nodes not found" << std::endl;
        std::cout << "[DATA_INIT][WAY] " << _way_counter << " ways parsed" << std::endl;

        if (!_meta[_file_no]) {
            _way_count[_file_no] = _way_counter;
        }
    }
    for (unsigned int _file_no = 0; _file_no < _file_count; _file_no++) {
        // ---------- FILE READING ----------
        char *_filepath = __filepath[_file_no];
        pugi::xml_document _doc;
        pugi::xml_parse_result _result = _doc.load_file(_filepath);
        if (!_result) {
            std::cout << "[DATA_INIT_PHASE3][FILE][E] Error: " << _result.description() << " when loading " << _filepath << std::endl;
            continue;
        }
        std::cout
            << "[DATA_INIT_PHASE3][FILE] File " << _filepath << " loaded successfully" << std::endl;
        // ---------- RELATION PARSING ----------
        std::cout << "[DATA_INIT][RELATION] Parsing relations..." << std::endl;
        uint64_t _relation_counter = 0, _relation_err_counter = 0, _relation_ok_counter = 0;
        Progress _progress3(_relation_count[_file_no]);
        for (pugi::xml_node _relation : _doc.child("osm").children("relation")) {
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
                    if (ways.find(_ref) == ways.end()) {
                        _relation_err_counter++;
                        if (_relation_err_counter < 3)
                            std::cout << "[DATA_INIT][RELATION][E] Error: Way " << _ref << " not found" << std::endl;
                        continue;
                    }
                    Way *_w = ways[_ref];
                    NodePtr _start, _end;
                    for (EdgePtr _e : _w->edges) {
                        if (_start.node == nullptr) {
                            _start = *(_e.edge->start);
                        } else {
                            _end = *(_e.edge->end);
                            if (_start.node->id == _end.node->id) {
                                continue;
                            }
                            //_start.node->computed_edges.push_back(new ComputedEdge(_start, _end, {false, false, false, _type & 8 ? true : false, _type & 16 ? true : false}, _type & 8 ? 50 : 80, _route_name));
                            _start.node->push_relation(_id, _end.node, {false, false, false, _type & 8 ? true : false, _type & 16 ? true : false}, _type & 8 ? 60 : 80, _route_name);
                        }
                    }
                    _relation_ok_counter++;
                }
            }
            _relation_counter++;
            if (_meta) _progress3.prog(_relation_counter);
        }
        _progress3.done();
        if (_relation_err_counter > 2)
            std::cout << "[DATA_INIT][RELATION][E] ...\n"
                      << "[DATA_INIT][RELATION][E] Error: Total of " << _relation_err_counter << " ways not found" << std::endl;
        std::cout << "[DATA_INIT][RELATION] " << _relation_ok_counter << " ways parsed successfully" << std::endl;
        std::cout << "[DATA_INIT][RELATION] " << _relation_counter << " relations parsed" << std::endl;

        if (!_meta[_file_no]) {
            _relation_count[_file_no] = _relation_counter;
        }
    }
    std::cout << "[DATA_INIT][QUADTREE] Inserting nodes into quadtree..." << std::endl;
    Progress _progress3(nodes.size());
    for (auto n : nodes) {
        root->insert(n.second);
        _progress3.prog_delta(1);
    }
    _progress3.done();
    std::cout << "[DATA_INIT][QUADTREE] Nodes inserted into quadtree" << std::endl;

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

    // std::cout << "[DATA_INIT] Connecting Railway Stops to Nearest Pedestrain." << std::endl;
    // Progress _progress4(_railway_stops.size());
    // for (auto n : _railway_stops) {
    //     if (n == nullptr) continue;
    //     NodePtr nearest = root->find_nearest_node(n->lat, n->lon, [](const NodePtr &n) { return n.node->pedestrian; });
    //     if (nearest.node != nullptr) {
    //         n->computed_edges.push_back(new ComputedEdge(n, nearest, {true, false, false, false, false}, 10, nullptr, 200));
    //         nearest.node->computed_edges.push_back(new ComputedEdge(nearest, n, {true, false, false, false, false}, 10, nullptr, 200));
    //     }
    //     _progress4.prog_delta(1);
    // }
    // _progress4.done();

    delete[] _node_count;
    delete[] _way_count;
    delete[] _meta;
    return true;
}