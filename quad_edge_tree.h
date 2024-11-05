#ifndef QUAD_EDGE_TREE_H
#define QUAD_EDGE_TREE_H
#include <set>
#include "edge.h"
#include "node.h"
#include <unordered_map>

#define MAX_EDGE_IN_TREENODE 1000

class QuadEdgeTreeNode {
private:
    int x;
    int y;
    int z;
    double lat_min, lat_max, lon_min, lon_max;
    bool splitted = false;
    std::multiset<EdgePtr> edges_display;
    QuadEdgeTreeNode *nw, *ne, *sw, *se;
    void split();
    void push_to_children(EdgePtr e);

public:
    QuadEdgeTreeNode(int x, int y, int z);
    QuadEdgeTreeNode(int x, int y, int z, double lat_min, double lat_max, double lon_min, double lon_max);
    void insert(EdgePtr e);
    std::multiset<EdgePtr> get_edges();
    std::multiset<EdgePtr> get_edges(int x, int y, int z);
    std::multiset<EdgePtr> get_edges(int x, int y, int z);
};
#endif