#ifndef ALGO_H
#define ALGO_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include "edge.h"
#include "node.h"
#include "utils.h"
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/heap/priority_queue.hpp>
#include <ankerl/unordered_dense.h>

#define BATCH_SIZE_MASK 0x08

enum zoneFilterMode { ALL,
                      BLACKLIST,
                      WHITELIST };

struct heapElement {
    float first;
    Node* second;
    bool operator<(const heapElement& other) const {
        return first > other.first;
    }
};

struct heapElementHeuristic {
    float first;
    Node* second;
    float third;
    bool operator<(const heapElementHeuristic& other) const {
        return first > other.first;
    }
};

struct NodeDetails {
    float distance;
    Node* parent;
    ComputedEdge* edge;
};

struct coord {
    double x;
    double y;
};

class Zone {
public:
    std::vector<coord> points;
    bool isIn(coord p);
};

class DijkstraPathFinder {
private:
    boost::heap::priority_queue<heapElement> pq;

protected:
    Node* start;
    Node* end;
    bool found;
    float distance;
    float travel_time;
    int method;
    int key;
    //boost::unordered_set<Node*> visited_nodes;
    ankerl::unordered_dense::set<Node*, ankerl::unordered_dense::hash<Node*>> visited_nodes;
    std::vector<ComputedEdge*> path;
    // boost::unordered_map<Node*, float> distance_map;
    // boost::unordered_map<Node*, Node*> parent_map;
    // boost::unordered_map<Node*, ComputedEdge*> edge_map;
    //boost::unordered_map<Node*, NodeDetails> details_map;
    ankerl::unordered_dense::map<Node*, NodeDetails, ankerl::unordered_dense::hash<Node*>> details_map;

public:
    DijkstraPathFinder(Node* start, Node* end, int method, int key);
    virtual void find_path();
    std::vector<ComputedEdge*> get_path();
    float get_distance();
    float get_travel_time();
    ankerl::unordered_dense::set<Node*, ankerl::unordered_dense::hash<Node*>> get_visited_nodes();
    std::vector<Node*> get_convex_hull_of_visited_nodes();
    std::vector<Node*> get_convex_hull_of(std::vector<Node*> v_nodes_unsorted);
    virtual ~DijkstraPathFinder() = default;
};

/**
 * @brief Class representing a Dijkstra path finder with heuristic optimization
 * @details For example: if you want to find a path from Shanghai to Beijing, the dijkstra will run with a order of time-increasing, which means if it reaches Hebei, it also reach Guangdong at the same time. It is expecting a path from Guangdong to Beijing with distance of almost 0. However it is just impossible. So we need to add a heuristic factor to make the dijkstra run with a order of "expected-"distance-increasing. Which means avoiding 南辕北辙.
 */
class HeuristicOptimizedDijkstraPathFinder : public DijkstraPathFinder {
protected:
    float heuristicFactor;
    float avgSpeed;
    boost::heap::priority_queue<heapElementHeuristic> pq_heuristic;
    constexpr float get_heuristic_time(float distance, Node* middle, Node* end) noexcept(true);
    constexpr float get_heuristic_distance(float distance, Node* middle, Node* end) noexcept(true);
    constexpr static float get_avg_speed(int method) noexcept(true);

public:
    /**
 * @param key: 0 for time, 1 for distance
 */
    HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, int key, float heuristicFactor);
    virtual void find_path() override;
};

class BidirectionalHODPF : public HeuristicOptimizedDijkstraPathFinder {
protected:
    boost::heap::priority_queue<heapElementHeuristic> pq_heuristic_end;
    // boost::unordered_map<Node*, float> distance_map_end;
    // boost::unordered_map<Node*, Node*> parent_map_end;
    // boost::unordered_map<Node*, ComputedEdge*> edge_map_end;
    // boost::unordered_map<Node*, NodeDetails> details_map_end;
    // boost::unordered_map<Node*, NodeDetails, boost::hash<Node*>> details_map_end;
    ankerl::unordered_dense::map<Node*, NodeDetails, ankerl::unordered_dense::hash<Node*>> details_map_end;
    float distance_end;
    float travel_time_end;
    std::vector<ComputedEdge*> path_end;
    // boost::unordered_set<Node*> visited_nodes_end;
    ankerl::unordered_dense::set<Node*, ankerl::unordered_dense::hash<Node*>> visited_nodes_end;

public:
    BidirectionalHODPF(Node* start, Node* end, int method, int key, float heuristicFactor);
    virtual void find_path() override;
    ankerl::unordered_dense::set<Node*, ankerl::unordered_dense::hash<Node*>> get_visited_nodes_end();
    std::vector<Node*> get_convex_hull_of_visited_nodes_end();
};

class ZonePathFinder : public BidirectionalHODPF {
private:
    Zone zone_;
    zoneFilterMode mode_;

public:
    ZonePathFinder(Node* start, Node* end, int method, int key, float heuristicFactor, Zone zone, zoneFilterMode mode);
    virtual void find_path() override;
};

#endif