#include "algo.h"

DijkstraPathFinder::DijkstraPathFinder(Node* start, Node* end, int method) : start(start), end(end), found(false), distance(1e18), travel_time(1e18), method(method) {}
std::vector<ResultEdge*> DijkstraPathFinder::get_path() {
    return path;
}
double DijkstraPathFinder::get_distance() {
    return distance;
}
double DijkstraPathFinder::get_travel_time() {
    return travel_time;
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
            double new_distance = distance_map[current] + e->getTravelTime(method);
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

HeuristicOptimizedDijkstraPathFinder::HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, double heuristicFactor) : DijkstraPathFinder(start, end, method), heuristicFactor(heuristicFactor) {}
double HeuristicOptimizedDijkstraPathFinder::get_heuristic(double _distance, Node* middle, Node* end) {
    return _distance + heuristicFactor * (middle->distance(*end)) / get_avg_speed(method);
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
        if (visited_nodes_heuristic.find(current) != visited_nodes_heuristic.end()) {
            continue;
        }
        visited_nodes_heuristic.insert(current);
        if (current == end) {
            found = true;
            break;
        }
        for (auto e : current->computed_edges) {
            Node* next = e->end->node;
            if (visited_nodes_heuristic.find(next) != visited_nodes_heuristic.end()) {
                continue;
            }
            if (!e->vis(method)) {
                continue;
            }
            double new_distance = distance_map[current] + e->getTravelTime(method);
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