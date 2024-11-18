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
    std::priority_queue<std::pair<float, Node*>> pq;

protected:
    Node* start;
    Node* end;
    bool found;
    float distance;
    float travel_time;
    int method;
    int key;
    std::unordered_set<Node*> visited_nodes;
    std::vector<ComputedEdge*> path;
    std::unordered_map<Node*, float> distance_map;
    std::unordered_map<Node*, Node*> parent_map;
    std::unordered_map<Node*, ComputedEdge*> edge_map;

public:
    DijkstraPathFinder(Node* start, Node* end, int method, int key);
    virtual void find_path();
    std::vector<ComputedEdge*> get_path();
    float get_distance();
    float get_travel_time();
    std::unordered_set<Node*> get_visited_nodes();
    std::vector<Node*> get_convex_hull_of_visited_nodes();
    virtual ~DijkstraPathFinder() = default;
};

/**
 * @brief Class representing a Dijkstra path finder with heuristic optimization
 * @details For example: if you want to find a path from Shanghai to Beijing, the dijkstra will run with a order of time-increasing, which means if it reaches Hebei, it also reach Guangdong at the same time. It is expecting a path from Guangdong to Beijing with distance of almost 0. However it is just impossible. So we need to add a heuristic factor to make the dijkstra run with a order of "expected-"distance-increasing. Which means avoiding 南辕北辙.
 */
class HeuristicOptimizedDijkstraPathFinder : public DijkstraPathFinder {
private:
    float heuristicFactor;
    std::priority_queue<std::pair<float, Node*>> pq_heuristic;
    float get_heuristic_time(float distance, Node* middle, Node* end);
    float get_heuristic_distance(float distance, Node* middle, Node* end);
    static float get_avg_speed(int method);

public:
    /**
 * @param key: 0 for time, 1 for distance
 */
    HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, int key, float heuristicFactor);
    void find_path() override;
};

#endif