#include <unordered_map>
#include "node.h"
#include "way.h"
#include <cstdint>

std::unordered_map<uint64_t, Node *> nodes;
std::unordered_map<uint64_t, Way *> ways;
uint64_t id_counter = 1145141919810ull;