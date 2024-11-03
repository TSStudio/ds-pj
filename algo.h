#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include "edge.h"
#include "node.h"

extern std::unordered_map<uint64_t, Node*> nodes;

class DijkstraPathFinder {
private:
    std::unordered_set<Node*> visited_nodes;
    std::priority_queue<std::pair<double, Node*>> pq;

protected:
    bool found;
    int method;
    Node* start;
    Node* end;
    std::vector<ResultEdge*> path;
    std::unordered_map<Node*, double> distance_map;
    std::unordered_map<Node*, Node*> parent_map;
    std::unordered_map<Node*, ResultEdge*> edge_map;
    double distance;
    double travel_time;

public:
    DijkstraPathFinder(Node* start, Node* end, int method);
    virtual void find_path();
    std::vector<ResultEdge*> get_path();
    double get_distance();
    double get_travel_time(int method);
};

#endif