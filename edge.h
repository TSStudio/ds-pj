#ifndef EDGE_H
#define EDGE_H

#include <string>
#include <cstdint>

class NodePtr;
class allowance {
public:
    bool pedestrian;
    bool bicycle;
    bool car;
    bool bus;
    bool subway;
    bool transfer;
};
/**
 * Definition of road levels:
 * 0: highway=motorway //display all the time
 * 8: highway=trunk //display at level 8 or higher
 * 10: highway=primary //display at level 10 or higher
 * 12: highway=secondary //display at level 12 or higher
 * 14: highway=tertiary //display at level 14 or higher
 * 16: highway=unclassified //display at level 16 or higher
 * 16: highway=residential //display at level 16 or higher
 */

/**
 * @brief Class representing an edge in the map
 */
class EdgeUtil {
public:
    /**
     * @brief Get the level of the road
     */
    static int getLevel(const std::string &highway) {
        if (highway == "motorway" || highway == "motorway_link") {
            return 0;
        } else if (highway == "trunk" || highway == "trunk_link") {
            return 8;
        } else if (highway == "primary" || highway == "primary_link") {
            return 10;
        } else if (highway == "secondary" || highway == "secondary_link") {
            return 12;
        } else if (highway == "tertiary" || highway == "tertiary_link") {
            return 14;
        } else if (highway == "unclassified") {
            return 16;
        } else if (highway == "residential") {
            return 16;
        } else {
            return 16;
        }
    }
    /**
     * @brief Get the allowance of the road
     */
    static allowance getAllowance(const std::string &highway) {
        if (highway == "motorway" || highway == "motorway_link") {
            return {false, false, true, false, false, false};
        } else if (highway == "trunk" || highway == "trunk_link") {
            return {false, false, true, false, false, false};
        } else if (highway == "primary" || highway == "primary_link") {
            return {true, true, true, false, false, false};
        } else if (highway == "secondary" || highway == "secondary_link") {
            return {true, true, true, false, false, false};
        } else if (highway == "tertiary" || highway == "tertiary_link") {
            return {true, true, true, false, false, false};
        } else if (highway == "unclassified") {
            return {true, true, true, false, false, false};
        } else if (highway == "residential") {
            return {true, true, true, false, false, false};
        } else if (highway == "living_street") {
            return {true, true, false, false, false, false};
        } else if (highway == "pedestrian") {
            return {true, true, false, false, false, false};
        } else if (highway == "footway") {
            return {true, false, false, false, false, false};
        } else if (highway == "cycleway") {
            return {false, true, false, false, false, false};
        } else if (highway == "path") {
            return {true, true, false, false, false, false};
        } else if (highway == "steps") {
            return {true, false, false, false, false, false};
        } else {
            return {true, false, false, false, false, false};
        }
    }
    /**
     * @brief Get the default speed limit of the road
     */
    static double getDefaultSpeedLimit(const std::string &highway) {
        if (highway == "motorway" || highway == "motorway_link") {
            return 33.3;
        } else if (highway == "trunk" || highway == "trunk_link") {
            return 20;
        } else if (highway == "primary" || highway == "primary_link") {
            return 14;
        } else if (highway == "secondary" || highway == "secondary_link") {
            return 13;
        } else if (highway == "tertiary" || highway == "tertiary_link") {
            return 12;
        } else if (highway == "unclassified") {
            return 10;
        } else if (highway == "residential") {
            return 10;
        } else {
            return 5;
        }
    }
};

/**
 * @brief Class representing an edge in the map
 */
class Edge {
public:
    NodePtr *start;        //start node
    NodePtr *end;          //end node
    double distance;       //distance between the nodes
    float distance_f;      //distance between the nodes
    bool isRoad;           //is a road
    int appear_level_min;  //minimum appearance level of the edge
    bool virtual_edge = false;
    uint64_t belong_to_way = 0;
    uint64_t seq = 0;
    int fill = 0;  //-1 left, 0 none, 1 right
    char *edgetype;
    Edge(NodePtr start, NodePtr end, double distance, bool isRoad, int appear_level_min);  //constructor
    Edge();                                                                                //default constructor
    Edge(NodePtr start, NodePtr end, bool isRoad, int appear_level_min);
};

/**
 * @brief Shell of Edge* for comparison
 */
class EdgePtr {
public:
    Edge *edge;
    EdgePtr(Edge *e) : edge(e) {}
    EdgePtr() : edge(nullptr) {}
    bool operator==(const EdgePtr &other) const;
    bool operator<(const EdgePtr &other) const;
    double distance() const;
};

/**
 * @brief Class to really compute shortest path, not to display
 */
class ComputedEdge {
public:
    NodePtr *start;
    NodePtr *end;
    allowance allow;
    double speed_limit;
    char *name;
    double forceTime;
    double distance;
    float distance_f;
    int methodUsed;
    ComputedEdge(NodePtr start, NodePtr end, allowance allow, double speed_limit, char *name, double forceTime = 0);
    double getTravelTime(int method);  //bit 0: pedestrian, bit 1: bicycle, bit 2: car, bit 3: bus, bit 4: subway
    float getTravelTimeF(int method);
    double getDistance(int method);
    float getDistanceF(int method);
    bool vis(int method);
};

class ResultEdge : public ComputedEdge {
public:
    ResultEdge(NodePtr start, NodePtr end, allowance allow, double speed_limit, char *name, double forceTime = 0);
    ResultEdge(ComputedEdge *e, int method);
    int method;
};

EdgePtr deepCopyEdge(EdgePtr e);
EdgePtr deepCopyEdge(Edge *e);
#endif