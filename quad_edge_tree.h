#ifndef QUAD_EDGE_TREE_H
#define QUAD_EDGE_TREE_H
#include <set>
#include "edge.h"
#include "node.h"
#include <unordered_map>
#include <iostream>

#define MAX_EDGE_IN_TREENODE 1000
#define MAX_Z 18

class QuadEdgeTreeNode {
private:
    int x;
    int y;
    int z;
    double lat_min, lat_max, lon_min, lon_max;
    double lat_mid, lon_mid;
    bool splitted = false;
    std::multiset<EdgePtr> edges_display;
    QuadEdgeTreeNode *nw, *ne, *sw, *se;
    double get_lat_mid();
    void split();
    void push_to_children(EdgePtr e);

public:
    QuadEdgeTreeNode(int x, int y, int z, double lat_min, double lat_max, double lon_min, double lon_max);
    void insert(EdgePtr e);
    std::multiset<EdgePtr> get_edges();
    std::multiset<EdgePtr> get_edges(int x, int y, int z);
};
#endif