#include "algo.h"

DijkstraPathFinder::DijkstraPathFinder(Node* start, Node* end, int method) : start(start), end(end), found(false), distance(1e18), travel_time(1e18), method(method) {}
std::vector<ComputedEdge*> DijkstraPathFinder::get_path() {
    return path;
}
double DijkstraPathFinder::get_distance() {
    return distance;
}
double DijkstraPathFinder::get_travel_time(int method) {
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
                edge_map[next] = e;
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