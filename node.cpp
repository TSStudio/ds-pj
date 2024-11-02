#include "node.h"

Node::Node(uint64_t id, double lat, double lon) : id(id), lat(lat), lon(lon) {
    level = 999;
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
double Node::distance(double lat, double lon) const {
    long double lat1 = this->lat * PI / 180.0L;
    long double lon1 = this->lon * PI / 180.0L;
    long double lat2 = lat * PI / 180.0L;
    long double lon2 = lon * PI / 180.0L;
    return (double)(EARTH_RADIUS * acosl(sinl(lat1) * sinl(lat2) + cosl(lat1) * cosl(lat2) * cosl(lon1 - lon2)));
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
double NodePtr::distance(const NodePtr &other) const {
    return node->distance(*other.node);
}
double NodePtr::distance(double lat, double lon) const {
    return node->distance(lat, lon);
}