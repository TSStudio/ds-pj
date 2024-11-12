#include <unordered_map>
#include "node.h"
#include "way.h"
#include "quad_tree.h"
#include "quad_edge_tree.h"
#include <cstdint>

std::unordered_map<uint64_t, Node *> nodes;
std::unordered_map<uint64_t, Way *> ways;
QuadTreeNode *root = new QuadTreeNode(-90, 90, -180, 180);
QuadEdgeTreeNode *root_edge = new QuadEdgeTreeNode(0, 0, 0, -85.05112877980659, 85.05112877980659, -180, 180);
uint64_t id_counter = 1145141919810ull;
bool __debug = false;