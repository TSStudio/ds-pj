#ifndef WAY_H
#define WAY_H

#include <cstdint>
#include <string>
#include <vector>

class Way {
public:
    uint64_t id;
    std::vector<uint64_t> members;
    std::string type;
    Way(uint64_t id, std::string type) : id(id), type(type) {}
    Way() : id(0), members(std::vector<uint64_t>()), type("") {}
    bool operator==(const Way &other) const {
        return id == other.id;
    }
};
#endif