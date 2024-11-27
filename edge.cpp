#include "edge.h"
#include "node.h"
Edge::Edge(NodePtr _start, NodePtr _end, double _distance, bool _isRoad, int _appear_level_min) : distance(_distance), isRoad(_isRoad), appear_level_min(_appear_level_min), virtual_edge(false) {
    this->start = new NodePtr(_start);
    this->end = new NodePtr(_end);
    this->distance_f = _distance;
}
Edge::Edge() : start(nullptr), end(nullptr), distance(0), distance_f(0), isRoad(false), appear_level_min(0) {}
Edge::Edge(NodePtr start, NodePtr end, bool isRoad, int appear_level_min) : isRoad(isRoad), appear_level_min(appear_level_min) {
    distance = start.distance(end);
    distance_f = distance;
    this->start = new NodePtr(start);
    this->end = new NodePtr(end);
}

bool EdgePtr::operator==(const EdgePtr &other) const {
    return edge->start->node->id == other.edge->start->node->id && edge->end->node->id == other.edge->end->node->id;
}
bool EdgePtr::operator<(const EdgePtr &other) const {
    return edge->appear_level_min < other.edge->appear_level_min;
}
double EdgePtr::distance() const {
    return edge->distance;
}

ComputedEdge::ComputedEdge(NodePtr start, NodePtr end, allowance allow, double speed_limit, char *name, double forceTime) : speed_limit(speed_limit), name(name), forceTime(forceTime) {
    this->start = start.node;
    this->end = end.node;
    this->allow = allow;
    distance = start.distance(end);
    distance_f = distance;
}

bool ComputedEdge::vis(int method) noexcept(true) {
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
    } else if ((method & 16 && allow.transfer) || (method & 8 && allow.transfer)) {
        return true;
    } else {
        return false;
    }
}

double ComputedEdge::getTravelTime(int method) noexcept(true) {
    methodUsed = 1;
    [[unlikely]]
    if (forceTime > 1)
        return forceTime;
    double speed = 0;  //m/s
    [[likely]]
    if (method & 4 && allow.car) {
        speed = 0.9 * speed_limit;
        methodUsed = 4;
        return distance / speed;
    }
    if (method & 16 && allow.subway) {
        speed = 0.5 * speed_limit;
        methodUsed = 16;
        return distance / speed;
    }
    if (method & 8 && allow.bus) {
        speed = 4.9;
        methodUsed = 8;
        return distance / speed;
    }
    if (method & 2 && allow.bicycle) {
        speed = 3;
        methodUsed = 2;
        return distance / speed;
    }
    if (method & 1 && allow.pedestrian) {
        if (method & 8)
            speed = 0.9;
        else
            speed = 1.2;
        methodUsed = 1;
        return distance / speed;
    }
    return 1e18;
}

float ComputedEdge::getTravelTimeF(int method) noexcept(true) {
    methodUsed = 1;
    [[unlikely]]
    if (forceTime > 1)
        return forceTime;
    float speed = 0;  //m/s
    [[likely]]
    if (method & 4 && allow.car) {
        speed = 0.9 * speed_limit;
        methodUsed = 4;
        return distance / speed;
    }
    if (method & 16 && allow.subway) {
        speed = 0.5 * speed_limit;
        methodUsed = 16;
        return distance / speed;
    }
    if (method & 8 && allow.bus) {
        speed = 4.9;
        methodUsed = 8;
        return distance / speed;
    }
    if (method & 2 && allow.bicycle) {
        speed = 3;
        methodUsed = 2;
        return distance / speed;
    }
    if (method & 1 && allow.pedestrian) {
        if (method & 8)
            speed = 0.9;
        else
            speed = 1.2;
        methodUsed = 1;
        return distance / speed;
    }
    return 1e18;
}

double ComputedEdge::getDistance(int method) noexcept(true) {
    if (vis(method)) return distance;
    return 1e18;
}
float ComputedEdge::getDistanceF(int method) noexcept(true) {
    if (vis(method)) return distance;
    return 1e18;
}

EdgePtr deepCopyEdge(EdgePtr e) {
    return EdgePtr(new Edge(*e.edge));
}

EdgePtr deepCopyEdge(Edge *e) {
    return EdgePtr(new Edge(*e));
}