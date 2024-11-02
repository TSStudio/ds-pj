#include "quad_tree.h"

void QuadTreeNode::enable_split() {
    split = true;
    double lat_mid = (lat_min + lat_max) / 2;
    double lon_mid = (lon_min + lon_max) / 2;
    sw = new QuadTreeNode(lat_min, lat_mid, lon_min, lon_mid);
    se = new QuadTreeNode(lat_min, lat_mid, lon_mid, lon_max);
    nw = new QuadTreeNode(lat_mid, lat_max, lon_min, lon_mid);
    ne = new QuadTreeNode(lat_mid, lat_max, lon_mid, lon_max);
    push_to_children();
}

void QuadTreeNode::push_to_children(NodePtr n) {
    if (!split) {
        return;
    }
    if (n.node->lat > (lat_min + lat_max) / 2) {
        if (n.node->lon < (lon_min + lon_max) / 2) {
            nw->insert(n);
        } else {
            ne->insert(n);
        }
    } else {
        if (n.node->lon < (lon_min + lon_max) / 2) {
            sw->insert(n);
        } else {
            se->insert(n);
        }
    }
}

void QuadTreeNode::push_to_children() {
    if (!split) {
        return;
    }
    for (auto n : nodes) {
        push_to_children(n);
    }
    nodes.clear();
}

QuadTreeNode::QuadTreeNode(double lat_min, double lat_max, double lon_min, double lon_max) : lat_min(lat_min), lat_max(lat_max), lon_min(lon_min), lon_max(lon_max) {
    nw = ne = sw = se = nullptr;
}

void QuadTreeNode::insert(NodePtr n) {
    if (n.node->lat < lat_min || n.node->lat > lat_max || n.node->lon < lon_min || n.node->lon > lon_max) {
        return;
    }

    if (!split && nodes.size() > MAX_POINT_IN_TREENODE) {
        nodes.insert(n);
        enable_split();
    } else if (!split) {
        nodes.insert(n);
    } else {
        push_to_children(n);
    }
}

std::multiset<NodePtr> QuadTreeNode::get_nodes() {
    if (!split) {
        return nodes;
    }
    std::multiset<NodePtr> result;
    std::multiset<NodePtr> temp;
    temp = nw->get_nodes();
    result.insert(temp.begin(), temp.end());
    temp = ne->get_nodes();
    result.insert(temp.begin(), temp.end());
    temp = sw->get_nodes();
    result.insert(temp.begin(), temp.end());
    temp = se->get_nodes();
    result.insert(temp.begin(), temp.end());
    return result;
}

std::multiset<NodePtr> QuadTreeNode::get_nodes(double lat_min, double lat_max, double lon_min, double lon_max) {
    if (lat_min > this->lat_max || lat_max < this->lat_min || lon_min > this->lon_max || lon_max < this->lon_min) {
        return std::multiset<NodePtr>();
    }
    if (!split) {
        std::multiset<NodePtr> result;
        for (auto n : nodes) {
            if (n.node->lat >= lat_min && n.node->lat <= lat_max && n.node->lon >= lon_min && n.node->lon <= lon_max) {
                result.insert(n);
            }
        }
        return result;
    }
    std::multiset<NodePtr> result;
    std::multiset<NodePtr> temp;
    temp = nw->get_nodes(lat_min, lat_max, lon_min, lon_max);
    result.insert(temp.begin(), temp.end());
    temp = ne->get_nodes(lat_min, lat_max, lon_min, lon_max);
    result.insert(temp.begin(), temp.end());
    temp = sw->get_nodes(lat_min, lat_max, lon_min, lon_max);
    result.insert(temp.begin(), temp.end());
    temp = se->get_nodes(lat_min, lat_max, lon_min, lon_max);
    result.insert(temp.begin(), temp.end());
    return result;
}

std::multiset<NodePtr> QuadTreeNode::get_nodes(double lat_min, double lat_max, double lon_min, double lon_max, int level) {
    if (lat_min > this->lat_max || lat_max < this->lat_min || lon_min > this->lon_max || lon_max < this->lon_min) {
        return std::multiset<NodePtr>();
    }
    if (!split) {
        std::multiset<NodePtr> result;
        for (auto n : nodes) {
            if (n.node->level > level) break;
            if (n.node->lat >= lat_min && n.node->lat <= lat_max && n.node->lon >= lon_min && n.node->lon <= lon_max) {
                result.insert(n);
            }
        }
        return result;
    }
    std::multiset<NodePtr> result;
    std::multiset<NodePtr> temp;
    temp = nw->get_nodes(lat_min, lat_max, lon_min, lon_max, level);
    result.insert(temp.begin(), temp.end());
    temp = ne->get_nodes(lat_min, lat_max, lon_min, lon_max, level);
    result.insert(temp.begin(), temp.end());
    temp = sw->get_nodes(lat_min, lat_max, lon_min, lon_max, level);
    result.insert(temp.begin(), temp.end());
    temp = se->get_nodes(lat_min, lat_max, lon_min, lon_max, level);
    result.insert(temp.begin(), temp.end());
    return result;
}

NodePtr QuadTreeNode::find_nearest_node(double lat, double lon, bool road) {
    if (!split) {
        NodePtr result;
        double min_distance = 1e18;
        for (auto n : nodes) {
            if (road && !n.node->road) {
                continue;
            }
            double d = n.distance(lat, lon);
            if (d < min_distance) {
                min_distance = d;
                result = n;
            }
        }
        return result;
    }
    NodePtr result;
    double min_distance = 1e18;
    NodePtr temp;
    //find in its children
    double min_distance_if_in_other_children = 1e18;
    double lat_div = (lat_min + lat_max) / 2;
    double lon_div = (lon_min + lon_max) / 2;
    min_distance_if_in_other_children = std::min(NodeUtil::distance(lat, lon, lat, lon_div), min_distance_if_in_other_children);
    min_distance_if_in_other_children = std::min(NodeUtil::distance(lat, lon, lat_div, lon), min_distance_if_in_other_children);
    if (lat > lat_div) {
        if (lon < (lon_min + lon_max) / 2) {
            temp = nw->find_nearest_node(lat, lon);
        } else {
            temp = ne->find_nearest_node(lat, lon);
        }
    } else {
        if (lon < lon_div) {
            temp = sw->find_nearest_node(lat, lon);
        } else {
            temp = se->find_nearest_node(lat, lon);
        }
    }

    if (temp.node != nullptr) {
        double d = temp.distance(lat, lon);
        if (d < min_distance) {
            min_distance = d;
            result = temp;
            if (min_distance < min_distance_if_in_other_children) {
                return result;
            }
        }
    }
    temp = nw->find_nearest_node(lat, lon);
    if (temp.node != nullptr) {
        double d = temp.distance(lat, lon);
        if (d < min_distance) {
            min_distance = d;
            result = temp;
        }
    }
    temp = ne->find_nearest_node(lat, lon);
    if (temp.node != nullptr) {
        double d = temp.distance(lat, lon);
        if (d < min_distance) {
            min_distance = d;
            result = temp;
        }
    }
    temp = sw->find_nearest_node(lat, lon);
    if (temp.node != nullptr) {
        double d = temp.distance(lat, lon);
        if (d < min_distance) {
            min_distance = d;
            result = temp;
        }
    }
    temp = se->find_nearest_node(lat, lon);
    if (temp.node != nullptr) {
        double d = temp.distance(lat, lon);
        if (d < min_distance) {
            min_distance = d;
            result = temp;
        }
    }
    return result;
}