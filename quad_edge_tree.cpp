#include "quad_edge_tree.h"

extern std::unordered_map<uint64_t, Node*> nodes;
extern uint64_t id_counter;
extern bool __debug;

QuadEdgeTreeNode::QuadEdgeTreeNode(int x, int y, int z, double lat_min, double lat_max, double lon_min, double lon_max) : x(x), y(y), z(z), lat_min(lat_min), lat_max(lat_max), lon_min(lon_min), lon_max(lon_max) {
    nw = ne = sw = se = nullptr;
    lat_mid = get_lat_mid();
    lon_mid = (lon_min + lon_max) / 2;
}
double QuadEdgeTreeNode::get_lat_mid() {
    double n = M_PI - (2 * M_PI * (2 * y + 1)) / pow(2, z + 1);
    return (180 / M_PI) * atan(0.5 * (exp(n) - exp(-n)));
}
void QuadEdgeTreeNode::insert(EdgePtr e) {
    EdgePtr ed = deepCopyEdge(e);
    edges_display.insert(ed);
    if (splitted) {
        push_to_children(ed);
    } else if (edges_display.size() > MAX_EDGE_IN_TREENODE && z < MAX_Z) {
        split();
    }
}
void QuadEdgeTreeNode::split() {
    if (__debug) {
        std::cout << "[D]splitting " << x << " " << y << " " << z << std::endl;
        std::cout << "[D] edge lat min max lon min max: " << lat_min << " " << lat_max << " " << lon_min << " " << lon_max << std::endl;
    }
    splitted = true;
    sw = new QuadEdgeTreeNode(x * 2, y * 2 + 1, z + 1, lat_min, lat_mid, lon_min, lon_mid);
    se = new QuadEdgeTreeNode(x * 2 + 1, y * 2 + 1, z + 1, lat_min, lat_mid, lon_mid, lon_max);
    nw = new QuadEdgeTreeNode(x * 2, y * 2, z + 1, lat_mid, lat_max, lon_min, lon_mid);
    ne = new QuadEdgeTreeNode(x * 2 + 1, y * 2, z + 1, lat_mid, lat_max, lon_mid, lon_max);
    for (auto e : edges_display) {
        push_to_children(e);
    }
}
void QuadEdgeTreeNode::push_to_children(EdgePtr e) {
    if (!splitted) {
        return;
    }
    //check if fully in the child
    bool in_both_north_south_childs = false;
    bool in_both_east_west_childs = false;
    if (e.edge->start->node->lat < lat_mid && e.edge->end->node->lat > lat_mid) {
        in_both_north_south_childs = true;
    } else if (e.edge->start->node->lat > lat_mid && e.edge->end->node->lat < lat_mid) {
        in_both_north_south_childs = true;
    }
    if (e.edge->start->node->lon < lon_mid && e.edge->end->node->lon > lon_mid) {
        in_both_east_west_childs = true;
    } else if (e.edge->start->node->lon > lon_mid && e.edge->end->node->lon < lon_mid) {
        in_both_east_west_childs = true;
    }
    if (!in_both_east_west_childs && !in_both_north_south_childs) {
        if (e.edge->start->node->lat > lat_mid) {
            if (e.edge->start->node->lon < lon_mid) {
                nw->insert(e);
            } else {
                ne->insert(e);
            }
        } else {
            if (e.edge->start->node->lon < lon_mid) {
                sw->insert(e);
            } else {
                se->insert(e);
            }
        }
        return;
    }
    //split edge then push to children
    NodePtr* point_across_east_west = nullptr;
    NodePtr* point_across_north_south = nullptr;
    double lat1 = e.edge->start->node->lat;
    double lon1 = e.edge->start->node->lon;
    double lat2 = e.edge->end->node->lat;
    double lon2 = e.edge->end->node->lon;
    if (in_both_east_west_childs) {
        double lat_across = lat1 + (lat2 - lat1) * (lon_mid - lon1) / (lon2 - lon1);
        double lon_across = lon_mid;
        Node* n = new Node(id_counter++, lat_across, lon_across);
        nodes[n->id] = n;
        point_across_east_west = new NodePtr(n);
    }
    if (in_both_north_south_childs) {
        double lon_across = lon1 + (lon2 - lon1) * (lat_mid - lat1) / (lat2 - lat1);
        double lat_across = lat_mid;
        Node* n = new Node(id_counter++, lat_across, lon_across);
        nodes[n->id] = n;
        point_across_north_south = new NodePtr(n);
    }
    if (in_both_east_west_childs && !in_both_north_south_childs) {
        EdgePtr e1 = deepCopyEdge(e);
        e1.edge->end = point_across_east_west;
        //check if north or south
        if (e.edge->start->node->lat > lat_mid) {
            if (e.edge->start->node->lon < lon_mid) {
                nw->insert(e1);
            } else {
                ne->insert(e1);
            }
        } else {
            if (e.edge->start->node->lon < lon_mid) {
                sw->insert(e1);
            } else {
                se->insert(e1);
            }
        }
        EdgePtr e2 = deepCopyEdge(e);
        e2.edge->start = point_across_east_west;
        //check if north or south
        if (e.edge->end->node->lat > lat_mid) {
            if (e.edge->end->node->lon < lon_mid) {
                nw->insert(e2);
            } else {
                ne->insert(e2);
            }
        } else {
            if (e.edge->end->node->lon < lon_mid) {
                sw->insert(e2);
            } else {
                se->insert(e2);
            }
        }
    }
    if (in_both_north_south_childs && !in_both_east_west_childs) {
        EdgePtr e1 = deepCopyEdge(e);
        e1.edge->end = point_across_north_south;
        //check if east or west
        if (e.edge->start->node->lon < lon_mid) {
            if (e.edge->start->node->lat > lat_mid) {
                nw->insert(e1);
            } else {
                sw->insert(e1);
            }
        } else {
            if (e.edge->start->node->lat > lat_mid) {
                ne->insert(e1);
            } else {
                se->insert(e1);
            }
        }
        EdgePtr e2 = deepCopyEdge(e);
        e2.edge->start = point_across_north_south;
        //check if east or west
        if (e.edge->end->node->lon < lon_mid) {
            if (e.edge->end->node->lat > lat_mid) {
                nw->insert(e2);
            } else {
                sw->insert(e2);
            }
        } else {
            if (e.edge->end->node->lat > lat_mid) {
                ne->insert(e2);
            } else {
                se->insert(e2);
            }
        }
    }
    if (in_both_north_south_childs && in_both_east_west_childs) {
        //check who is closer to start
        double dist1 = e.edge->start->distance(*point_across_east_west);
        double dist2 = e.edge->start->distance(*point_across_north_south);
        EdgePtr e1 = deepCopyEdge(e);
        EdgePtr e2 = deepCopyEdge(e);
        EdgePtr e3 = deepCopyEdge(e);
        if (dist1 < dist2) {
            e1.edge->end = point_across_east_west;
            e2.edge->start = point_across_east_west;
            e2.edge->end = point_across_north_south;
            e3.edge->start = point_across_north_south;
        } else {
            e1.edge->end = point_across_north_south;
            e2.edge->start = point_across_north_south;
            e2.edge->end = point_across_east_west;
            e3.edge->start = point_across_east_west;
        }
        //check if north or south
        if (e.edge->start->node->lat > lat_mid) {
            if (e.edge->start->node->lon < lon_mid) {
                nw->insert(e1);
            } else {
                ne->insert(e1);
            }
        } else {
            if (e.edge->start->node->lon < lon_mid) {
                sw->insert(e1);
            } else {
                se->insert(e1);
            }
        }
        //same to e3
        if (e.edge->end->node->lat > lat_mid) {
            if (e.edge->end->node->lon < lon_mid) {
                nw->insert(e3);
            } else {
                ne->insert(e3);
            }
        } else {
            if (e.edge->end->node->lon < lon_mid) {
                sw->insert(e3);
            } else {
                se->insert(e3);
            }
        }
        //need some consideration for e2
        double lat_tot = e2.edge->start->node->lat + e2.edge->end->node->lat;  // one should be lat_mid
        double lon_tot = e2.edge->start->node->lon + e2.edge->end->node->lon;  // one should be lon_mid
        if (lat_tot > 2 * lat_mid) {
            if (lon_tot > 2 * lon_mid) {
                ne->insert(e2);
            } else {
                nw->insert(e2);
            }
        } else {
            if (lon_tot > 2 * lon_mid) {
                se->insert(e2);
            } else {
                sw->insert(e2);
            }
        }
    }
}
std::multiset<EdgePtr> QuadEdgeTreeNode::get_edges() {
    return edges_display;
}
std::multiset<EdgePtr> QuadEdgeTreeNode::get_edges(int _x, int _y, int _z) {
    if (__debug) {
        std::cout << "[D]getting edges from " << x << " " << y << " " << z << " to " << _x << " " << _y << " " << _z << std::endl;
    }
    if (_z < z) {
        return std::multiset<EdgePtr>();
    }
    int xx = _x;
    int yy = _y;
    xx >>= (_z - z);
    yy >>= (_z - z);
    if (xx != x || yy != y) {
        return std::multiset<EdgePtr>();
    }
    if (!splitted) {
        if (__debug) {
            std::cout << "[D]returning edges from " << x << " " << y << " " << z << std::endl;
        }
        return edges_display;
    }
    if (_z == z) {
        if (__debug) {
            std::cout << "[D]returning edges from " << x << " " << y << " " << z << std::endl;
        }
        return edges_display;
    }
    //check in which child it is
    int x_rem = (_x >> (_z - z - 1)) - 2 * x;
    int y_rem = (_y >> (_z - z - 1)) - 2 * y;
    if (x_rem & 1) {
        if (y_rem & 1) {
            return se->get_edges(_x, _y, _z);
        } else {
            return ne->get_edges(_x, _y, _z);
        }
    } else {
        if (y_rem & 1) {
            return sw->get_edges(_x, _y, _z);
        } else {
            return nw->get_edges(_x, _y, _z);
        }
    }
}