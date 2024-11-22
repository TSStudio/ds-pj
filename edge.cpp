#include "edge.h"
#include "node.h"

extern std::unordered_set<ComputedEdge *> computed_edges_individual;

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
    this->start = new NodePtr(start);
    this->end = new NodePtr(end);
    this->allow = allow;
    distance = start.distance(end);
    distance_f = distance;
}
ComputedEdge::ComputedEdge(const ComputedEdge *other) : speed_limit(other->speed_limit), name(other->name), forceTime(other->forceTime) {
    this->start = new NodePtr(*other->start);
    this->end = new NodePtr(*other->end);
    this->allow = other->allow;
    distance = other->distance;
    distance_f = distance;
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
    } else if ((method & 16 && allow.transfer) || (method & 8 && allow.transfer)) {
        return true;
    } else {
        return false;
    }
}

double ComputedEdge::getTravelTime(int method) {
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
        speed = 0.4 * speed_limit;
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

float ComputedEdge::getTravelTimeF(int method) {
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
        speed = 0.4 * speed_limit;
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

double ComputedEdge::getDistance(int method) {
    if (vis(method)) return distance;
    return 1e18;
}
float ComputedEdge::getDistanceF(int method) {
    if (vis(method)) return distance;
    return 1e18;
}

ComputedEdgeSet::ComputedEdgeSet(ComputedEdge *upgrade_from) : ComputedEdge(*upgrade_from) {
    edges.push_back(upgrade_from);
}

bool ComputedEdgeSet::isNodeForwardable(NodePtr n) {
    return n.node->computed_edges.size() == 1 && n.node->computed_edges_in.size() == 1;
}

bool ComputedEdgeSet::isEdgeForwardable(ComputedEdge *e) {
    return e->allow == allow && e->speed_limit == speed_limit && e->name == name && e->forceTime == forceTime;
}

void ComputedEdgeSet::forward_extend() {
    NodePtr *n = end;
    while (isNodeForwardable(*n)) {
        ComputedEdge *e = n->node->computed_edges[0];
        if (!computed_edges_individual.contains(e)) {
            break;
        }
        if (!isEdgeForwardable(e)) {
            break;
        }
        if (e->isInSet) {
            break;
        }
        n->node->is_inner_node_of_set = true;
        n->node->is_inner_of = this;
        edges.push_back(e);
        n = e->end;
        e->isInSet = true;
        distance += e->distance;
        distance_f = distance;
    }
    end = n;
}

void ComputedEdgeSet::backward_extend() {
    NodePtr *n = start;
    while (isNodeForwardable(*n)) {
        ComputedEdge *e = n->node->computed_edges_in[0];
        if (!computed_edges_individual.contains(e)) {
            break;
        }
        if (!isEdgeForwardable(e)) {
            break;
        }
        if (e->isInSet) {
            break;
        }
        n->node->is_inner_node_of_set = true;
        n->node->is_inner_of = this;
        edges.push_front(e);
        n = e->start;
        e->isInSet = true;
        distance += e->distance;
        distance_f = distance;
    }
    start = n;
}

void ComputedEdgeSet::extend() {
    forward_extend();
    backward_extend();
}

EdgePtr deepCopyEdge(EdgePtr e) {
    return EdgePtr(new Edge(*e.edge));
}

EdgePtr deepCopyEdge(Edge *e) {
    return EdgePtr(new Edge(*e));
}