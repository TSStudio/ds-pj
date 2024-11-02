#include "edge.h"
#include "node.h"
Edge::Edge(NodePtr start, NodePtr end, double distance, bool isRoad, int appear_level_min) : distance(distance), isRoad(isRoad), appear_level_min(appear_level_min) {
    this->start = new NodePtr(start);
    this->end = new NodePtr(end);
}
Edge::Edge() : start(nullptr), end(nullptr), distance(0), isRoad(false), appear_level_min(0) {}
Edge::Edge(NodePtr start, NodePtr end, bool isRoad, int appear_level_min) : distance(distance), isRoad(isRoad), appear_level_min(appear_level_min) {
    distance = start.distance(end);
    this->start = new NodePtr(start);
    this->end = new NodePtr(end);
}

bool EdgePtr::operator==(const EdgePtr &other) const {
    return edge->start->node->id == other.edge->start->node->id && edge->end->node->id == other.edge->end->node->id;
}
bool EdgePtr::operator<(const EdgePtr &other) const {
    return edge->appear_level_min < other.edge->appear_level_min;
}
double EdgePtr::distance(const EdgePtr &other) const {
    return edge->distance;
}

ComputedEdge::ComputedEdge(NodePtr start, NodePtr end, allowance allow, double speed_limit) : speed_limit(speed_limit) {
    this->start = new NodePtr(start);
    this->end = new NodePtr(end);
    this->allow = allow;
    distance = start.distance(end);
}

bool ComputedEdge::vis(int method) {
    if (method & 1 && allow.pedestrian) {
        return true;
    } else if (method & 2 && allow.bicycle) {
        return true;
    } else if (method & 4 && allow.car) {
        return true;
    } else if (method & 8 && allow.bus) {
        return true;
    } else if (method & 16 && allow.subway) {
        return true;
    } else {
        return false;
    }
}

double ComputedEdge::getTravelTime(int method) {
    double speed = 0;  //m/s
    if (method & 1 && allow.pedestrian) {
        speed = 1.2;
    } else if (method & 2 && allow.bicycle) {
        speed = 3;
    } else if (method & 4 && allow.car) {
        speed = 0.5 * speed_limit;
    } else if (method & 8 && allow.bus) {
        speed = 0.4 * speed_limit;
    } else if (method & 16 && allow.subway) {
        speed = 0.4 * speed_limit;
    } else {
        speed = 0.0;
    }
    if (speed == 0.0) return 1e18;
    return distance / speed;
}