#ifndef NODE_H
#define NODE_H

#define EARTH_RADIUS 6371000.00L
#define EARTH_RADIUS_D 6371000.00
#define EARTH_RADIUS_F 6371000.00f
#define PI 3.14159265358979323846L
#define PI_D 3.14159265358979323846
#define PI_F 3.14159265358979323846f

#include <cstdint>
#include <cmath>
#include <vector>
#include <set>
#include "edge.h"
#include <unordered_map>

/**
 * @brief Utility class for node operations
 */
class NodeUtil {
public:
    /**
     * @brief Calculate the distance between two points on the Earth's surface
     */
    static double distance(double lat1, double lon1, double lat2, double lon2) {
        long double lat1_rad = lat1 * PI / 180.0L;
        long double lon1_rad = lon1 * PI / 180.0L;
        long double lat2_rad = lat2 * PI / 180.0L;
        long double lon2_rad = lon2 * PI / 180.0L;
        return (double)(EARTH_RADIUS * acosl(sinl(lat1_rad) * sinl(lat2_rad) + cosl(lat1_rad) * cosl(lat2_rad) * cosl(lon1_rad - lon2_rad)));
    }
};

/**
 * @brief Class representing a node in the map
 */
class Node {
public:
    uint64_t id;                                     //unique id
    double lat;                                      //latitude
    double lon;                                      //longitude
    bool road;                                       //is a start of a road, false if only end of a one-way road
    bool pedestrian;                                 //is a pedestrian node
    int level;                                       //minimum appearance level of the node
    bool bus_stop = false;                           //is a bus stop
    bool virtual_node = false;                       //is a virtual node
    char *name = nullptr;                            //name of the node
    std::multiset<EdgePtr> edges;                    //edges connected to the node
    std::vector<ComputedEdge *> computed_edges;      //Computed edges starting from the node
    std::vector<ComputedEdge *> computed_edges_end;  //Computed edges ending at the node
    std::unordered_map<uint64_t, Node *> vpoints;    //virtual point for relation

    Node(uint64_t id, double lat, double lon);                           //constructor
    Node();                                                              //default constructor
    bool operator==(const Node &other) const;                            //equality operator
    bool operator<(const Node &other) const;                             //less than operator
    double distance(const Node &other) const noexcept(true);             //distance to another node
    float distanceF(const Node &other) const noexcept(true);             //distance to another node
    float approxDistanceF(const Node &other) const noexcept(true);       //approximate distance to another node
    double distance(double lat, double lon) const noexcept(true);        //distance to a point
    float distanceF(double lat, double lon) const noexcept(true);        //distance to a point
    float approxDistanceF(double lat, double lon) const noexcept(true);  //approximate distance to a point

    void push_relation(uint64_t relation_id, Node *n, allowance allow, double speed_limit, char *name);  //push a relation
};

/**
 * @brief Shell of Node* for comparison
 */
class NodePtr {
public:
    Node *node;
    NodePtr(Node *n);
    NodePtr();
    NodePtr(const NodePtr &other);
    bool operator==(const NodePtr &other) const;
    bool operator<(const NodePtr &other) const;
    NodePtr &operator=(const NodePtr &other);
    double distance(const NodePtr &other) const;
    double distance(double lat, double lon) const;
};

#endif