#ifndef QUAD_TREE_H
#define QUAD_TREE_H
#include <set>
#include "node.h"
#include <iostream>
#define MAX_POINT_IN_TREENODE 1000

class QuadTreeNode {
private:
    bool split = false;
    QuadTreeNode *nw, *ne, *sw, *se;
    double lat_min, lat_max, lon_min, lon_max;
    std::multiset<NodePtr> nodes;
    void enable_split();
    void push_to_children();
    void push_to_children(NodePtr n);

public:
    QuadTreeNode(double lat_min, double lat_max, double lon_min, double lon_max);
    void insert(NodePtr n);
    std::multiset<NodePtr> get_nodes();
    std::multiset<NodePtr> get_nodes(double lat_min, double lat_max, double lon_min, double lon_max);
    std::multiset<NodePtr> get_nodes(double lat_min, double lat_max, double lon_min, double lon_max, int level);
    NodePtr find_nearest_node(double lat, double lon, bool road = true);
    NodePtr find_nearest_node(double lat, double lon, bool (*filter)(const NodePtr &));
};
#endif