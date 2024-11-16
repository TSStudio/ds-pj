#include "algo.h"

DijkstraPathFinder::DijkstraPathFinder(Node* start, Node* end, int method, int key) : start(start), end(end), found(false), distance(1e18), travel_time(1e18), method(method), key(key) {}
std::vector<ResultEdge*> DijkstraPathFinder::get_path() {
    return path;
}
double DijkstraPathFinder::get_distance() {
    return distance;
}
double DijkstraPathFinder::get_travel_time() {
    return travel_time;
}

std::unordered_set<Node*> DijkstraPathFinder::get_visited_nodes() {
    return visited_nodes;
}

std::vector<Node*> DijkstraPathFinder::get_convex_hull_of_visited_nodes() {
    std::vector<Node*> v_nodes_sorted;
    for (auto node : visited_nodes) {
        v_nodes_sorted.push_back(node);
    }
    std::sort(v_nodes_sorted.begin(), v_nodes_sorted.end(), [](Node* a, Node* b) {
        return a->lat < b->lat || (a->lat == b->lat && a->lon < b->lon);
    });
    //use Andrew
    // Andrew's monotone chain algorithm to find the convex hull
    std::vector<Node*> convex_hull;
    auto n = v_nodes_sorted.size();
    if (n < 3) return convex_hull;

    auto cross = [](Node* O, Node* A, Node* B) {
        return (A->lon - O->lon) * (B->lat - O->lat) - (A->lat - O->lat) * (B->lon - O->lon);
    };

    // Build lower hull
    for (int i = 0; i < n; ++i) {
        while (convex_hull.size() >= 2 && cross(convex_hull[convex_hull.size() - 2], convex_hull.back(), v_nodes_sorted[i]) <= 0) {
            convex_hull.pop_back();
        }
        convex_hull.push_back(v_nodes_sorted[i]);
    }

    // Build upper hull
    for (int i = n - 1, t = convex_hull.size() + 1; i >= 0; --i) {
        while (convex_hull.size() >= t && cross(convex_hull[convex_hull.size() - 2], convex_hull.back(), v_nodes_sorted[i]) <= 0) {
            convex_hull.pop_back();
        }
        convex_hull.push_back(v_nodes_sorted[i]);
    }

    convex_hull.pop_back();  // Remove the last point because it is the same as the first one
    return convex_hull;
}

void DijkstraPathFinder::find_path() {
    distance_map[start] = 0;
    pq.push(std::make_pair(0, start));
    while (!pq.empty()) {
        Node* current = pq.top().second;
        pq.pop();
        if (visited_nodes.find(current) != visited_nodes.end()) {
            continue;
        }
        visited_nodes.insert(current);
        if (current == end) {
            found = true;
            break;
        }
        for (auto e : current->computed_edges) {
            Node* next = e->end->node;
            if (visited_nodes.find(next) != visited_nodes.end()) {
                continue;
            }
            if (!e->vis(method)) {
                continue;
            }
            double new_distance = distance_map[current] + (key == 0 ? e->getTravelTime(method) : e->getDistance(method));
            if (distance_map.find(next) == distance_map.end() || new_distance < distance_map[next]) {
                distance_map[next] = new_distance;
                parent_map[next] = current;
                edge_map[next] = new ResultEdge(e, e->getMethodUsed(method));
                pq.push(std::make_pair(-new_distance, next));
            }
        }
    }

    if (found) {
        double dis = 0;
        travel_time = distance_map[end];
        Node* current = end;
        while (current != start) {
            Node* parent = parent_map[current];
            path.push_back(edge_map[current]);
            dis += edge_map[current]->distance;
            current = parent;
        }
        std::reverse(path.begin(), path.end());
        distance = dis;
    }
}

DijkstraPathFinder::~DijkstraPathFinder() {
    for (auto e : edge_map) {
        delete e.second;
    }
}

HeuristicOptimizedDijkstraPathFinder::HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, int key, double heuristicFactor) : DijkstraPathFinder(start, end, method, key), heuristicFactor(heuristicFactor) {}
double HeuristicOptimizedDijkstraPathFinder::get_heuristic(double _distance, Node* middle, Node* end) {
    if (key == 0)
        return _distance + heuristicFactor * (middle->distance(*end)) / get_avg_speed(method);
    else
        return _distance + heuristicFactor * (middle->distance(*end));
}

double HeuristicOptimizedDijkstraPathFinder::get_avg_speed(int method) {
    double speed = 0;  //m/s
    if (method & 1) {
        if (method & 8)
            speed = 0.9;
        else
            speed = 1.2;
    }
    if (method & 2) {
        speed = 3;
    }
    if (method & 4) {
        speed = 15;
    }
    if (method & 8) {
        speed = 7;
    }
    if (method & 16) {
        speed = 10;
    }
    if (speed == 0.0) return 1e18;
    return speed;
}

void HeuristicOptimizedDijkstraPathFinder::find_path() {
    distance_map[start] = 0;
    pq_heuristic.push(std::make_pair(0, start));
    while (!pq_heuristic.empty()) {
        Node* current = pq_heuristic.top().second;
        pq_heuristic.pop();
        if (visited_nodes.find(current) != visited_nodes.end()) {
            continue;
        }
        visited_nodes.insert(current);
        if (current == end) {
            found = true;
            break;
        }
        for (auto e : current->computed_edges) {
            Node* next = e->end->node;
            if (visited_nodes.find(next) != visited_nodes.end()) {
                continue;
            }
            if (!e->vis(method)) {
                continue;
            }
            double new_distance = distance_map[current] + (key == 0 ? e->getTravelTime(method) : e->getDistance(method));
            if (distance_map.find(next) == distance_map.end() || new_distance < distance_map[next]) {
                distance_map[next] = new_distance;
                parent_map[next] = current;
                edge_map[next] = new ResultEdge(e, e->getMethodUsed(method));
                pq_heuristic.push(std::make_pair(-get_heuristic(new_distance, next, end), next));
            }
        }
    }

    if (found) {
        double dis = 0;
        travel_time = distance_map[end];
        Node* current = end;
        while (current != start) {
            Node* parent = parent_map[current];
            path.push_back(edge_map[current]);
            dis += edge_map[current]->distance;
            current = parent;
        }
        std::reverse(path.begin(), path.end());
        distance = dis;
    }
}