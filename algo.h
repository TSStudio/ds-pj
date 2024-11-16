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
    std::priority_queue<std::pair<double, Node*>> pq;

protected:
    Node* start;
    Node* end;
    bool found;
    double distance;
    double travel_time;
    int method;
    int key;
    std::unordered_set<Node*> visited_nodes;
    std::vector<ResultEdge*> path;
    std::unordered_map<Node*, double> distance_map;
    std::unordered_map<Node*, Node*> parent_map;
    std::unordered_map<Node*, ResultEdge*> edge_map;

public:
    DijkstraPathFinder(Node* start, Node* end, int method, int key);
    virtual void find_path();
    std::vector<ResultEdge*> get_path();
    double get_distance();
    double get_travel_time();
    std::unordered_set<Node*> get_visited_nodes();
    std::vector<Node*> get_convex_hull_of_visited_nodes();
    virtual ~DijkstraPathFinder();
};

/**
 * @brief Class representing a Dijkstra path finder with heuristic optimization
 * @details For example: if you want to find a path from Shanghai to Beijing, the dijkstra will run with a order of time-increasing, which means if it reaches Hebei, it also reach Guangdong at the same time. It is expecting a path from Guangdong to Beijing with distance of almost 0. However it is just impossible. So we need to add a heuristic factor to make the dijkstra run with a order of "expected-"distance-increasing. Which means avoiding 南辕北辙.
 */
class HeuristicOptimizedDijkstraPathFinder : public DijkstraPathFinder {
private:
    double heuristicFactor;
    std::priority_queue<std::pair<double, Node*>> pq_heuristic;
    double get_heuristic(double distance, Node* middle, Node* end);
    static double get_avg_speed(int method);

public:
    /**
 * @param key: 0 for time, 1 for distance
 */
    HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, int key, double heuristicFactor);
    void find_path() override;
};

#endif