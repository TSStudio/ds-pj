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

ComputedEdge::ComputedEdge(NodePtr start, NodePtr end, allowance allow, double speed_limit, char *name, double forceTime) : speed_limit(speed_limit), name(name), forceTime(forceTime) {
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
    if (forceTime > 1) return forceTime;
    double speed = 0;  //m/s
    if (method & 1 && allow.pedestrian) {
        if (method & 8)
            speed = 0.9;
        else
            speed = 1.2;
    }
    if (method & 2 && allow.bicycle) {
        speed = 3;
    }
    if (method & 4 && allow.car) {
        speed = 0.9 * speed_limit;
    }
    if (method & 8 && allow.bus) {
        speed = 0.4 * speed_limit;
    }
    if (method & 16 && allow.subway) {
        speed = 0.5 * speed_limit;
    }
    if (speed == 0.0) return 1e18;
    return distance / speed;
}

ResultEdge::ResultEdge(NodePtr start, NodePtr end, allowance allow, double speed_limit, char *name, double forceTime) : ComputedEdge(start, end, allow, speed_limit, name, forceTime) {}
ResultEdge::ResultEdge(ComputedEdge *e, int method) : ComputedEdge(*e->start, *e->end, e->allow, e->speed_limit, e->name, e->forceTime), method(method) {}

int ComputedEdge::getMethodUsed(int method) {
    if (method & 16 && allow.subway) return 16;
    if (method & 8 && allow.bus) return 8;
    if (method & 4 && allow.car) return 4;
    if (method & 2 && allow.bicycle) return 2;
    if (method & 1 && allow.pedestrian) return 1;
    return 0;
}