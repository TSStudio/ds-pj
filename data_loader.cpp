#include "data_loader.h"

std::unordered_map<uint64_t, Node *> nodes;
//std::unordered_map<uint64_t, Way *> ways;

QuadTreeNode *root = new QuadTreeNode(-90, 90, -180, 180);

bool data_init(char *_filepath) {
    pugi::xml_document _doc;
    pugi::xml_parse_result _result = _doc.load_file(_filepath);
    if (!_result) {
        std::cout << "[DATA_INIT][FILE][E] Error: " << _result.description() << std::endl;
        return false;
    }
    std::cout
        << "[DATA_INIT][FILE] File loaded successfully" << std::endl;

    std::cout << "[DATA_INIT][NODE] Parsing nodes..." << std::endl;
    uint64_t _node_count = 0;
    for (pugi::xml_node _node : _doc.child("osm").children("node")) {
        uint64_t _id = _node.attribute("id").as_ullong();
        double _lat = _node.attribute("lat").as_double();
        double _lon = _node.attribute("lon").as_double();
        Node *_n = new Node(_id, _lat, _lon);
        nodes[_id] = _n;
        // root->insert(_n);
        _node_count++;
    }
    std::cout << "[DATA_INIT][NODE] " << _node_count << " nodes parsed successfully" << std::endl;

    std::cout << "[DATA_INIT][WAY] Parsing ways..." << std::endl;
    uint64_t _way_count = 0, _way_err_count = 0;
    for (pugi::xml_node _way : _doc.child("osm").children("way")) {
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
        if (_speed_limit == 0) {
            _speed_limit = EdgeUtil::getDefaultSpeedLimit(_highway);
        }
        allowance _allow = EdgeUtil::getAllowance(_highway);

        NodePtr _start, _end;

        for (pugi::xml_node _nd : _way.children("nd")) {
            uint64_t _ref = _nd.attribute("ref").as_ullong();
            if (nodes.find(_ref) == nodes.end()) {
                _way_err_count++;
                if (_way_err_count < 3)
                    std::cout << "[DATA_INIT][WAY][E] Error: Node " << _ref << " not found" << std::endl;
                continue;
            }
            if (_start.node == nullptr) {
                _start = NodePtr(nodes[_ref]);
            } else {
                _end = NodePtr(nodes[_ref]);
                Edge *_e = new Edge(_start, _end, _isRoad, _appear_level_min);
                _start.node->edges.insert(EdgePtr(_e));
                _end.node->edges.insert(EdgePtr(_e));
                _start.node->level = std::min(_start.node->level, _appear_level_min);
                _end.node->level = std::min(_end.node->level, _appear_level_min);
                if (_isRoad) {
                    _start.node->road = true;
                }
                if (_direction & 1) {
                    _start.node->computed_edges.push_back(new ComputedEdge(_start, _end, _allow, _speed_limit));
                }
                if (_direction & 2) {
                    _end.node->computed_edges.push_back(new ComputedEdge(_end, _start, _allow, _speed_limit));
                }
                _start = _end;
            }

            //_w->members.push_back(_ref);
            //nodes[_ref]->ways.push_back(_w->id);
        }

        //ways[_w->id] = _w;
        _way_count++;
    }
    if (_way_err_count > 2)
        std::cout << "[DATA_INIT][WAY][E] ...\n"
                  << "[DATA_INIT][WAY][E] Error: Total of " << _way_err_count << " ways have at least one node not found" << std::endl;
    std::cout << "[DATA_INIT][WAY] " << _way_count << " ways parsed" << std::endl;

    std::cout << "[DATA_INIT][QUADTREE] Inserting nodes into quadtree..." << std::endl;
    Progress _progress(nodes.size());
    for (auto n : nodes) {
        root->insert(n.second);
        _progress.prog_delta(1);
    }
    std::cout << "[DATA_INIT][QUADTREE] Nodes inserted into quadtree" << std::endl;

    return true;
}