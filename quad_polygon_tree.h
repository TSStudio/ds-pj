#ifndef QUAD_EDGE_TREE_H
#define QUAD_EDGE_TREE_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

#define MAX_POLYGON_IN_TREENODE 100
#define MAX_Z 18

class PolygonNode {
public:
    double lat_;
    double lon_;
    uint64_t seq_;
    PolygonNode();
    PolygonNode(double _lat, double _lon);
    PolygonNode(uint64_t _seq, double _lat, double _lon);
};

class PolygonEdge {
public:
    PolygonNode start_;
    PolygonNode end_;
    bool intersect_;
    PolygonNode* intersectSeq_;
};

class Polygon {
private:
    uint64_t nodeSize_ = 0;
    std::vector<PolygonNode> nodes_;

public:
    Polygon();
    /**
     * @brief Split the polygon by latitude, not changing the original polygon, store the result in the target polygons
     */
    std::vector<std::pair<int, Polygon> > SplitByLat(double lat);
    /**
     * @brief Split the polygon by longitude, not changing the original polygon, store the result in the target polygons
     */
    std::vector<std::pair<int, Polygon> > SplitByLon(double lon);

    std::vector<std::pair<int, Polygon> > SplitByCustom(std::function<bool(PolygonNode, PolygonNode)> _intersectCheck, std::function<PolygonNode(PolygonNode, PolygonNode)> _intersectPoint);
    /**
     * @brief Split the polygon by latitude and longitude, not changing the original polygon, store the result in the target polygons
     */
    void SplitByLatLon(Polygon* _targetNorthEast, Polygon* _targetNorthWest, Polygon* _targetSouthEast, Polygon* _targetSouthWest);
    /**
     * @brief Append a node to the polygon
     */
    void PushNode(PolygonNode _node);
    void PushNode(double _lat, double _lon);
    uint64_t nodeSize();
};

class PolygonSplitting {
public:
    Polygon polygon_;
    int crossback_;
    bool side_;
};

class QuadPolygonTreeNode {
};
#endif