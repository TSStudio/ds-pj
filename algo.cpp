#include "algo.h"

bool Zone::isIn(coord p) {
    int n = points.size();
    int count = 0;
    for (int i = 0; i < n; i++) {
        const coord& p1 = points[i];
        const coord& p2 = points[(i + 1) % n];
        if (p1.y == p2.y) {
            if (p.y == p1.y) {
                if (std::min(p1.x, p2.x) <= p.x && p.x <= std::max(p1.x, p2.x)) {
                    return true;
                }
            }
            continue;
        }
        if (p.y < std::min(p1.y, p2.y) || p.y > std::max(p1.y, p2.y)) {
            continue;
        }
        double x = (p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
        if (x > p.x) {
            count++;
        }
    }
    return count % 2 == 1;
}

DijkstraPathFinder::DijkstraPathFinder(Node* start, Node* end, int method, int key) : start(start), end(end), found(false), distance(1e18), travel_time(1e18), method(method), key(key) {
    pq.reserve(1000);
    details_map.reserve(1000);
    visited_nodes.reserve(1000);
    path.reserve(1000);
}
auto DijkstraPathFinder::get_path() -> std::vector<ComputedEdge*> {
    return path;
}
float DijkstraPathFinder::get_distance() {
    return distance;
}
float DijkstraPathFinder::get_travel_time() {
    return travel_time;
}

auto DijkstraPathFinder::get_visited_nodes() -> ankerl::unordered_dense::set<Node*, ankerl::unordered_dense::hash<Node*>> {
    return visited_nodes;
}

auto DijkstraPathFinder::get_convex_hull_of_visited_nodes() -> std::vector<Node*> {
    return get_convex_hull_of(std::vector<Node*>(visited_nodes.begin(), visited_nodes.end()));
}

auto DijkstraPathFinder::get_convex_hull_of(std::vector<Node*> v_nodes_unsorted) -> std::vector<Node*> {
    std::vector<Node*> v_nodes_sorted;
    for (auto node : v_nodes_unsorted) {
        v_nodes_sorted.push_back(node);
    }
    std::sort(v_nodes_sorted.begin(), v_nodes_sorted.end(), [](Node* a, Node* b) {
        return a->lat < b->lat || (a->lat == b->lat && a->lon < b->lon);
    });
    // Andrew's monotone chain algorithm to find the convex hull
    std::vector<Node*> convex_hull;
    auto n = v_nodes_sorted.size();
    if (n < 3) return convex_hull;

    auto cross = [](Node* O, Node* A, Node* B) {
        return (A->lon - O->lon) * (B->lat - O->lat) - (A->lat - O->lat) * (B->lon - O->lon);
    };

    // Build lower hull

    // CAUTION UNDEFINED BEHAVIOR
    // converting size_t to int
    for (int i = 0; i < (int)n; ++i) {
        while (convex_hull.size() >= 2 && cross(convex_hull[convex_hull.size() - 2], convex_hull.back(), v_nodes_sorted[i]) <= 0) {
            convex_hull.pop_back();
        }
        convex_hull.push_back(v_nodes_sorted[i]);
    }

    // Build upper hull
    for (int i = n - 1, t = convex_hull.size() + 1; i >= 0; --i) {
        // CAUTION UNDEFINED BEHAVIOR
        // converting size_t to int
        while ((int)convex_hull.size() >= t && cross(convex_hull[convex_hull.size() - 2], convex_hull.back(), v_nodes_sorted[i]) <= 0) {
            convex_hull.pop_back();
        }
        convex_hull.push_back(v_nodes_sorted[i]);
    }

    convex_hull.pop_back();  // Remove the last point because it is the same as the first one
    return convex_hull;
}

void DijkstraPathFinder::find_path() {
    details_map[start] = {0, nullptr, nullptr};
    pq.push({0, start});
    while (!pq.empty()) {
        Node* current = pq.top().second;
        float cur_dis = pq.top().first;
        pq.pop();
        if (visited_nodes.contains(current)) {
            continue;
        }
        visited_nodes.insert(current);
        if (current == end) {
            found = true;
            break;
        }
        for (auto e : current->computed_edges) {
            Node* next = e->end;
            if (visited_nodes.contains(next)) {
                continue;
            }
            if (!e->vis(method)) {
                continue;
            }
            float new_distance = cur_dis + (key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method));
            if (!details_map.contains(next) || new_distance < details_map[next].distance) {
                // distance_map[next] = new_distance;
                // parent_map[next] = current;
                // edge_map[next] = e;
                details_map[next] = {new_distance, current, e};
                pq.push({new_distance, next});
            }
        }
    }

    if (found) {
        float dis = 0;
        travel_time = details_map[end].distance;
        Node* current = end;
        while (current != start) {
            Node* parent = details_map[current].parent;
            path.push_back(details_map[current].edge);
            dis += details_map[current].edge->distance;
            current = parent;
        }
        std::reverse(path.begin(), path.end());
        distance = dis;
    }
}

HeuristicOptimizedDijkstraPathFinder::HeuristicOptimizedDijkstraPathFinder(Node* start, Node* end, int method, int key, float heuristicFactor) : DijkstraPathFinder(start, end, method, key), heuristicFactor(heuristicFactor) {
    pq_heuristic.reserve(1000);
}

constexpr float HeuristicOptimizedDijkstraPathFinder::get_heuristic_time(float _distance, Node* middle, Node* end) noexcept(true) {
    return _distance + heuristicFactor * (middle->distanceF(*end)) / avgSpeed;
}

constexpr float HeuristicOptimizedDijkstraPathFinder::get_heuristic_distance(float _distance, Node* middle, Node* end) noexcept(true) {
    return _distance + heuristicFactor * middle->distanceF(*end);
}

constexpr float HeuristicOptimizedDijkstraPathFinder::get_avg_speed(int method) noexcept(true) {
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
        speed = 4.9;  //bus avg 4.9 m/s
    }
    if (method & 16) {
        speed = 10;
    }
    if (speed == 0.0) return 1e18;
    return speed;
}

void HeuristicOptimizedDijkstraPathFinder::find_path() {
    avgSpeed = get_avg_speed(method);
    details_map[start] = {0, nullptr, nullptr};
    pq_heuristic.push({0, start, 0});
    while (!pq_heuristic.empty()) {
        Node* current = pq_heuristic.top().second;
        float cur_dis = pq_heuristic.top().third;
        pq_heuristic.pop();
        if (visited_nodes.contains(current)) {
            continue;
        }
        visited_nodes.insert(current);
        [[unlikely]]
        if (current == end) {
            found = true;
            break;
        }
        for (auto& e : current->computed_edges) {
            Node* next = e->end;
            if (visited_nodes.contains(next)) {
                continue;
            }
            if (!e->vis(method)) {
                continue;
            }
            float new_distance = cur_dis + (key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method));
            if (!details_map.contains(next) || new_distance < details_map[next].distance) {
                details_map[next] = {new_distance, current, e};
                if (key == 0)
                    pq_heuristic.push({get_heuristic_time(new_distance, next, end), next, new_distance});
                else
                    pq_heuristic.push({get_heuristic_distance(new_distance, next, end), next, new_distance});
            }
        }
    }

    if (found) {
        float dis = 0;
        travel_time = details_map[end].distance;
        Node* current = end;
        while (current != start) {
            Node* parent = details_map[current].parent;
            path.push_back(details_map[current].edge);
            dis += details_map[current].edge->distance;
            current = parent;
        }
        std::reverse(path.begin(), path.end());
        distance = dis;
    }
}

BidirectionalHODPF::BidirectionalHODPF(Node* start, Node* end, int method, int key, float heuristicFactor) : HeuristicOptimizedDijkstraPathFinder(start, end, method, key, heuristicFactor) {
    details_map_end.reserve(1000);
    visited_nodes_end.reserve(1000);
    pq_heuristic_end.reserve(1000);
}

void BidirectionalHODPF::find_path() {
    avgSpeed = get_avg_speed(method);
    details_map[start] = {0, nullptr, nullptr};
    details_map_end[end] = {0, nullptr, nullptr};
    pq_heuristic.push({0, start, 0});
    pq_heuristic_end.push({0, end, 0});
    int i = 0;
    Node* middle = nullptr;
    while (!pq_heuristic.empty() && !pq_heuristic_end.empty()) {
        i++;
        if (i & BATCH_SIZE_MASK) {
            Node* current = pq_heuristic.top().second;
            float cur_dis = pq_heuristic.top().third;
            pq_heuristic.pop();
            if (visited_nodes.contains(current)) {
                continue;
            }
            visited_nodes.insert(current);
            [[unlikely]]
            if (current == end || visited_nodes_end.contains(current)) {
                found = true;
                middle = current;
                break;
            }
            for (const auto& e : current->computed_edges) {
                Node* next = e->end;
                if (visited_nodes.contains(next)) {
                    continue;
                }
                if (!e->vis(method)) {
                    continue;
                }
                float ddis = key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method);
                if (!e->lastGetSuccess) continue;
                float new_distance = cur_dis + ddis;
                if (!details_map.contains(next) || new_distance < details_map[next].distance) {
                    details_map[next] = {new_distance, current, e};
                    if (key == 0)
                        pq_heuristic.push({get_heuristic_time(new_distance, next, end), next, new_distance});
                    else
                        pq_heuristic.push({get_heuristic_distance(new_distance, next, end), next, new_distance});
                }
            }
        } else {
            Node* current = pq_heuristic_end.top().second;
            float cur_dis = pq_heuristic_end.top().third;
            pq_heuristic_end.pop();
            if (visited_nodes_end.contains(current)) {
                continue;
            }
            visited_nodes_end.insert(current);
            [[unlikely]]
            if (current == start || visited_nodes.contains(current)) {
                found = true;
                middle = current;
                break;
            }
            for (const auto& e : current->computed_edges_end) {
                Node* next = e->start;
                if (visited_nodes_end.contains(next)) {
                    continue;
                }
                float ddis = key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method);
                if (!e->lastGetSuccess) continue;
                float new_distance = cur_dis + ddis;
                if (!details_map_end.contains(next) || new_distance < details_map_end[next].distance) {
                    details_map_end[next] = {new_distance, current, e};
                    if (key == 0)
                        pq_heuristic_end.push({get_heuristic_time(new_distance, next, start), next, new_distance});
                    else
                        pq_heuristic_end.push({get_heuristic_distance(new_distance, next, start), next, new_distance});
                }
            }
        }
    }

    if (found) {
        if (middle == nullptr) {
            //this should not happen
            return;
        }
        float dis = 0;
        travel_time = details_map[middle].distance + details_map_end[middle].distance;
        Node* current = middle;
        while (current != start) {
            Node* parent = details_map[current].parent;
            path.push_back(details_map[current].edge);
            dis += details_map[current].edge->distance;
            current = parent;
        }
        std::reverse(path.begin(), path.end());
        current = middle;
        while (current != end) {
            Node* parent = details_map_end[current].parent;
            path.push_back(details_map_end[current].edge);
            dis += details_map_end[current].edge->distance;
            current = parent;
        }
        distance = dis;
    }
}

auto BidirectionalHODPF::get_visited_nodes_end() -> ankerl::unordered_dense::set<Node*, ankerl::unordered_dense::hash<Node*>> {
    return visited_nodes_end;
}

auto BidirectionalHODPF::get_convex_hull_of_visited_nodes_end() -> std::vector<Node*> {
    return get_convex_hull_of(std::vector<Node*>(visited_nodes_end.begin(), visited_nodes_end.end()));
}

ZonePathFinder::ZonePathFinder(Node* start, Node* end, int method, int key, float heuristicFactor, Zone zone, zoneFilterMode mode) : BidirectionalHODPF(start, end, method, key, heuristicFactor), zone_(zone), mode_(mode) {}

void ZonePathFinder::find_path() {
    avgSpeed = get_avg_speed(method);
    details_map[start] = {0, nullptr, nullptr};
    details_map_end[end] = {0, nullptr, nullptr};
    pq_heuristic.push({0, start, 0});
    pq_heuristic_end.push({0, end, 0});
    int i = 0;
    Node* middle = nullptr;
    while (!pq_heuristic.empty() && !pq_heuristic_end.empty()) {
        i++;
        if (i & BATCH_SIZE_MASK) {
            Node* current = pq_heuristic.top().second;
            float cur_dis = pq_heuristic.top().third;
            pq_heuristic.pop();
            if (visited_nodes.contains(current)) {
                continue;
            }
            visited_nodes.insert(current);
            [[unlikely]]
            if (current == end || visited_nodes_end.contains(current)) {
                found = true;
                middle = current;
                break;
            }
            for (auto& e : current->computed_edges) {
                Node* next = e->end;
                if (visited_nodes.contains(next)) {
                    continue;
                }
                if (!e->vis(method)) {
                    continue;
                }
                if (mode_ == zoneFilterMode::BLACKLIST) {
                    if (zone_.isIn({next->lat, next->lon})) {
                        continue;
                    }
                } else if (mode_ == zoneFilterMode::WHITELIST) {
                    if (!zone_.isIn({next->lat, next->lon})) {
                        continue;
                    }
                }
                float new_distance = cur_dis + (key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method));
                if (!details_map.contains(next) || new_distance < details_map[next].distance) {
                    details_map[next] = {new_distance, current, e};
                    if (key == 0)
                        pq_heuristic.push({get_heuristic_time(new_distance, next, end), next, new_distance});
                    else
                        pq_heuristic.push({get_heuristic_distance(new_distance, next, end), next, new_distance});
                }
            }
        } else {
            Node* current = pq_heuristic_end.top().second;
            float cur_dis = pq_heuristic_end.top().third;
            pq_heuristic_end.pop();
            if (visited_nodes_end.contains(current)) {
                continue;
            }
            visited_nodes_end.insert(current);
            [[unlikely]]
            if (current == start || visited_nodes.contains(current)) {
                found = true;
                middle = current;
                break;
            }
            for (auto& e : current->computed_edges_end) {
                Node* next = e->start;
                if (visited_nodes_end.contains(next)) {
                    continue;
                }
                if (!e->vis(method)) {
                    continue;
                }
                if (mode_ == zoneFilterMode::BLACKLIST) {
                    if (zone_.isIn({next->lat, next->lon})) {
                        continue;
                    }
                } else if (mode_ == zoneFilterMode::WHITELIST) {
                    if (!zone_.isIn({next->lat, next->lon})) {
                        continue;
                    }
                }
                float new_distance = cur_dis + (key == 0 ? e->getTravelTimeF(method) : e->getDistanceF(method));
                if (!details_map_end.contains(next) || new_distance < details_map_end[next].distance) {
                    details_map_end[next] = {new_distance, current, e};
                    if (key == 0)
                        pq_heuristic_end.push({get_heuristic_time(new_distance, next, start), next, new_distance});
                    else
                        pq_heuristic_end.push({get_heuristic_distance(new_distance, next, start), next, new_distance});
                }
            }
        }
    }

    if (found) {
        if (middle == nullptr) {
            //this should not happen
            return;
        }
        float dis = 0;
        travel_time = details_map[middle].distance + details_map_end[middle].distance;
        Node* current = middle;
        while (current != start) {
            Node* parent = details_map[current].parent;
            path.push_back(details_map[current].edge);
            dis += details_map[current].edge->distance;
            current = parent;
        }
        std::reverse(path.begin(), path.end());
        current = middle;
        while (current != end) {
            Node* parent = details_map_end[current].parent;
            path.push_back(details_map_end[current].edge);
            dis += details_map_end[current].edge->distance;
            current = parent;
        }
        distance = dis;
    }
}

namespace Salesman {
PathFinder::PathFinder(std::vector<Node*> nodes, int method, int key, float heuristicFactor) : nodes_(nodes), method_(method), key_(key), heuristicFactor_(heuristicFactor) {}

std::vector<Result> PathFinder::find_path() {
    for (auto begin : nodes_) {
        for (auto end : nodes_) {
            if (begin == end) continue;
            if (result_map.contains({begin, end})) continue;
            BidirectionalHODPF bidirectionalHODPF(begin, end, method_, key_, heuristicFactor_);
            bidirectionalHODPF.find_path();
            Result result;
            result.distance_ = bidirectionalHODPF.get_distance();
            result.path_ = bidirectionalHODPF.get_path();
            result_map[{begin, end}] = result;
        }
    }
    std::vector<Node*> Remaining;
    for (int i = 1; i < static_cast<int>(nodes_.size()); i++) {
        Remaining.push_back(nodes_[i]);
    }
    dfs(Remaining, 0, {nodes_[0]});
    std::vector<Result> results;
    for (auto it = current_optimal_result.begin(); it != current_optimal_result.end(); it++) {
        Result result;
        result.distance_ = 0;
        if (it != current_optimal_result.end() - 1) {
            result = result_map[{*it, *(it + 1)}];
        } else {
            result = result_map[{*it, current_optimal_result.front()}];
        }
        results.push_back(result);
    }
    return results;
}

std::vector<Result> PathFinder::find_path_no_return() {
    for (auto begin : nodes_) {
        for (auto end : nodes_) {
            if (begin == end) continue;
            if (result_map.contains({begin, end})) continue;
            BidirectionalHODPF bidirectionalHODPF(begin, end, method_, key_, heuristicFactor_);
            bidirectionalHODPF.find_path();
            Result result;
            result.distance_ = bidirectionalHODPF.get_distance();
            result.path_ = bidirectionalHODPF.get_path();
            result_map[{begin, end}] = result;
        }
    }
    Node* dummy_node = new Node();
    for (auto n : nodes_) {
        result_map[{n, dummy_node}].distance_ = 0;
        result_map[{dummy_node, n}].distance_ = 0;
    }
    std::vector<Node*> Remaining;
    for (int i = 0; i < static_cast<int>(nodes_.size()); i++) {
        Remaining.push_back(nodes_[i]);
    }
    dfs(Remaining, 0, {dummy_node});
    //find the dummy node in the path
    std::vector<Node*> rt;
    auto it = std::find(current_optimal_result.begin(), current_optimal_result.end(), dummy_node);
    for (auto i = it + 1; i != current_optimal_result.end(); i++) {
        rt.push_back(*i);
    }
    for (auto i = current_optimal_result.begin(); i != it; i++) {
        rt.push_back(*i);
    }
    std::vector<Result> results;
    for (auto it = rt.begin(); it != rt.end(); it++) {
        Result result;
        result.distance_ = 0;
        if (it != rt.end() - 1) {
            result = result_map[{*it, *(it + 1)}];
        } else {
            result = result_map[{*it, rt.front()}];
        }
        results.push_back(result);
    }
    return results;
}

void PathFinder::dfs(std::vector<Node*> Remaining, float cur_distance, std::vector<Node*> cur_path) {
    if (cur_distance > current_optimal_distance) return;
    if (Remaining.empty()) {
        //add the distance from the last node to the first node
        cur_distance += result_map[{cur_path.back(), cur_path.front()}].distance_;
        if (cur_distance < current_optimal_distance) {
            current_optimal_distance = cur_distance;
            current_optimal_result = cur_path;
        }
        return;
    }
    for (auto node : Remaining) {
        std::vector<Node*> newRemaining;
        for (auto n : Remaining) {
            if (n != node) {
                newRemaining.push_back(n);
            }
        }
        std::vector<Node*> newCurPath = cur_path;
        newCurPath.push_back(node);
        float newCurDistance = cur_distance;
        if (!cur_path.empty()) {
            newCurDistance += result_map[{cur_path.back(), node}].distance_;
        }
        dfs(newRemaining, newCurDistance, newCurPath);
    }
}

}  // namespace Salesman