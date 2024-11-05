#ifndef WAY_H
#define WAY_H

#include "edge.h"
#include <cstdint>
#include <string>
#include <vector>

class Way {
public:
    uint64_t id;
    std::vector<EdgePtr> edges;
    Way(uint64_t id) : id(id) {}
    Way() : id(0), edges(std::vector<EdgePtr>()) {}
    bool operator==(const Way &other) const {
        return id == other.id;
    }
};
#endif