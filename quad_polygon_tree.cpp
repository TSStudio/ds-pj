//#include "quad_polygon_tree.h"

/**
 * For Split Functions
    Create an empty list of output polygons
    Create an empty list of pending crossbacks (one for each polygon)
    Find all intersections between the polygon and the line.
    Sort them by position along the line.
    Pair them up as alternating entry/exit lines.
    Append a polygon to the output list and make it current.
    Walk the polygon. For each edge:
        Append the first point to the current polygon.
        If there is an intersection with the split line:
            Add the intersection point to the current polygon.
            Find the intersection point in the intersection pairs.
            Set its partner as the crossback point for the current polygon.
            If there is an existing polygon with a crossback for this edge:
                Set the current polygon to be that polygon.
            Else
                Append a new polygon and new crossback point to the output lists and make it current.
            Add the intersection point to the now current polygon.
 */
// std::vector<std::pair<int, Polygon>> Polygon::SplitByCustom(std::function<bool(PolygonNode, PolygonNode)> _intersectCheck, std::function<PolygonNode(PolygonNode, PolygonNode)> _intersectPoint) {
//     std::vector<std::pair<int, Polygon>> result;
//     PolygonNode start = nodes_[this->nodeSize_ - 1];
//     std::vector<PolygonNode*> intersects;
//     std::vector<PolygonEdge> edges;
//     int k = 0;
//     for (int i = 0; i < nodeSize_; i++) {
//         PolygonNode end = nodes_[i];
//         if (_intersectCheck(start, end)) {
//             PolygonNode* intersect = new PolygonNode(_intersectPoint(start, end));
//             edges.push_back({start, end, true, intersect});
//             intersects.push_back(intersect);
//         } else {
//             edges.push_back({start, end, false, nullptr});
//         }
//         start = end;
//     }
//     if (intersects.size() == 0) {
//         return result;
//     }
//     k = 0;
//     for (auto i : intersects) {
//         i->seq_ = k++;
//     }
//     std::vector<PolygonSplitting*> splitting_list;
//     PolygonSplitting* current = new PolygonSplitting();
//     splitting_list.push_back(current);
//     bool side = 0;
//     current->side_ = side;
//     for (auto e : edges) {
//         current->polygon_.PushNode(e.start_);
//         if (e.intersect_) {
//             side = !side;
//             current->polygon_.PushNode(*e.intersectSeq_);
//             current->crossback_ = e.intersectSeq_->seq_ & 1 ? e.intersectSeq_->seq_ - 1 : e.intersectSeq_->seq_ + 1;

//             bool flg = false;
//             for (auto s : splitting_list) {
//                 if (s->crossback_ == e.intersectSeq_->seq_) {
//                     current = s;
//                     flg = true;
//                     break;
//                 }
//             }
//             if (!flg) {
//                 current = new PolygonSplitting();
//                 splitting_list.push_back(current);
//                 current->side_ = side;
//             }

//             current->polygon_.PushNode(*e.intersectSeq_);
//         }
//     }
//     for (auto s : splitting_list) {
//         result.push_back({s->side_, s->polygon_});
//     }
//     for (auto i : intersects) {
//         delete i;
//     }
//     for (auto s : splitting_list) {
//         delete s;
//     }
//     return result;
// }