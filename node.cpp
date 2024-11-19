#include "node.h"

extern std::unordered_map<uint64_t, Node *> nodes;
extern uint64_t id_counter;

Node::Node(uint64_t id, double lat, double lon) : id(id), lat(lat), lon(lon), road(false), pedestrian(false), level(999), virtual_node(false) {
}
Node::Node() : id(0), lat(0), lon(0) {}
bool Node::operator==(const Node &other) const {
    return id == other.id;
}
bool Node::operator<(const Node &other) const {
    return level < other.level;
}
double Node::distance(const Node &other) const {
    long double lat1 = lat * PI / 180.0L;
    long double lon1 = lon * PI / 180.0L;
    long double lat2 = other.lat * PI / 180.0L;
    long double lon2 = other.lon * PI / 180.0L;
    return (double)(EARTH_RADIUS * acosl(sinl(lat1) * sinl(lat2) + cosl(lat1) * cosl(lat2) * cosl(lon1 - lon2)));
}
float Node::distanceF(const Node &other) const {
    double lat1 = lat * PI_D / 180.0;
    double lon1 = lon * PI_D / 180.0;
    double lat2 = other.lat * PI_D / 180.0;
    double lon2 = other.lon * PI_D / 180.0;
    return (float)(EARTH_RADIUS_D * acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2)));
}
float Node::approxDistanceF(const Node &other) const {
    double dx = lon - other.lon;
    double dy = lat - other.lat;
    return (float)(sqrt(dx * dx + dy * dy) * EARTH_RADIUS_D * PI_D / 180.0);
}
double Node::distance(double lat, double lon) const {
    long double lat1 = this->lat * PI / 180.0L;
    long double lon1 = this->lon * PI / 180.0L;
    long double lat2 = lat * PI / 180.0L;
    long double lon2 = lon * PI / 180.0L;
    return (double)(EARTH_RADIUS * acosl(sinl(lat1) * sinl(lat2) + cosl(lat1) * cosl(lat2) * cosl(lon1 - lon2)));
}
float Node::distanceF(double lat, double lon) const {
    double lat1 = this->lat * PI_D / 180.0;
    double lon1 = this->lon * PI_D / 180.0;
    double lat2 = lat * PI_D / 180.0;
    double lon2 = lon * PI_D / 180.0;
    return (float)(EARTH_RADIUS_D * acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2)));
}
float Node::approxDistanceF(double lat, double lon) const {
    double dx = this->lon - lon;
    double dy = this->lat - lat;
    return (float)(sqrt(dx * dx + dy * dy) * EARTH_RADIUS_D * PI_D / 180.0);
}
void Node::push_relation(uint64_t relation_id, Node *n, allowance allow, double speed_limit, char *name) {
    Node *virtual_point;
    Node *target_virtual_point;
    if (!vpoints.contains(relation_id)) {
        virtual_point = new Node(id_counter, lat, lon);
        virtual_point->name = this->name;
        nodes[id_counter] = virtual_point;
        vpoints[relation_id] = virtual_point;
        virtual_point->computed_edges.push_back(new ComputedEdge(virtual_point, this, {false, false, false, false, false, true}, 10, nullptr, 300));
        computed_edges.push_back(new ComputedEdge(this, virtual_point, {false, false, false, false, false, true}, 10, nullptr, 300));
        id_counter++;
    } else {
        virtual_point = vpoints[relation_id];
    }
    if (!n->vpoints.contains(relation_id)) {
        target_virtual_point = new Node(id_counter, n->lat, n->lon);
        target_virtual_point->name = this->name;
        nodes[id_counter] = target_virtual_point;
        n->vpoints[relation_id] = target_virtual_point;
        target_virtual_point->computed_edges.push_back(new ComputedEdge(target_virtual_point, n, {false, false, false, false, false, true}, 10, nullptr, 300));
        n->computed_edges.push_back(new ComputedEdge(n, target_virtual_point, {false, false, false, false, false, true}, 10, nullptr, 300));
        id_counter++;
    } else {
        target_virtual_point = n->vpoints[relation_id];
    }
    virtual_point->computed_edges.push_back(new ComputedEdge(virtual_point, target_virtual_point, allow, speed_limit, name));
}

NodePtr::NodePtr(Node *n) : node(n) {}
NodePtr::NodePtr() : node(nullptr) {}
NodePtr::NodePtr(const NodePtr &other) : node(other.node) {}
bool NodePtr::operator==(const NodePtr &other) const {
    return node->id == other.node->id;
}
bool NodePtr::operator<(const NodePtr &other) const {
    return node->level < other.node->level;
}
// NodePtr NodePtr::operator=(const NodePtr &other) const {
//     return NodePtr(other);
// }
// NodePtr &NodePtr::operator=(const NodePtr &other) const {
//     return *new NodePtr(other);
// }
double NodePtr::distance(const NodePtr &other) const {
    return node->distance(*other.node);
}
double NodePtr::distance(double lat, double lon) const {
    return node->distance(lat, lon);
}
NodePtr &NodePtr::operator=(const NodePtr &other) {
    if (this != &other) {
        node = other.node;
    }

    return *this;
}
