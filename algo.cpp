#include "algo.h"

DijkstraPathFinder::DijkstraPathFinder(Node* start, Node* end, int method, int key) : start(start), end(end), found(false), distance(1e18), travel_time(1e18), method(method), key(key) {
    distance_map.reserve(1000);
    parent_map.reserve(1000);
    edge_map.reserve(1000);
    visited_nodes.reserve(1000);
    path.reserve(1000);
}
std::vector<ComputedEdge*> DijkstraPathFinder::get_path() {
    return path;
}
float DijkstraPathFinder::get_distance() {
    return distance;
}
float DijkstraPathFinder::get_travel_time() {
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
            float new_distance = distance_map[current] + (key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method));
            if (distance_map.find(next) == distance_map.end() || new_distance < distance_map[next]) {
                distance_map[next] = new_distance;
                parent_map[next] = current;
                edge_map[next] = e;
                pq.push(std::make_pair(-new_distance, next));
            }
        }
    }

    if (found) {
        float dis = 0;
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

HeuristicOptimizedDijkstraPathFinder::HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, int key, float heuristicFactor) : DijkstraPathFinder(start, end, method, key), heuristicFactor(heuristicFactor) {}

float HeuristicOptimizedDijkstraPathFinder::get_heuristic_time(float _distance, Node* middle, Node* end) {
    return _distance + heuristicFactor * (middle->distanceF(*end)) / get_avg_speed(method);
}

float HeuristicOptimizedDijkstraPathFinder::get_heuristic_distance(float _distance, Node* middle, Node* end) {
    return _distance + heuristicFactor * middle->distanceF(*end);
}

float HeuristicOptimizedDijkstraPathFinder::get_avg_speed(int method) {
    float speed = 0;  //m/s
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
            float new_distance = distance_map[current] + (key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method));
            if (distance_map.find(next) == distance_map.end() || new_distance < distance_map[next]) {
                distance_map[next] = new_distance;
                parent_map[next] = current;
                edge_map[next] = e;
                if (key == 0)
                    pq_heuristic.push(std::make_pair(-get_heuristic_time(new_distance, next, end), next));
                else
                    pq_heuristic.push(std::make_pair(-get_heuristic_distance(new_distance, next, end), next));
            }
        }
    }

    if (found) {
        float dis = 0;
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