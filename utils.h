#ifndef UTILS_H
#define UTILS_H
#include <cstdint>
#include <iostream>
#include <format>
#include <chrono>
#include <string>
#include <print>

class Progress {
public:
    uint64_t used_microseconds = 0;
    Progress(uint64_t total);
    void prog(uint64_t current);
    void prog_delta(uint64_t delta);
    void print();
    void done();
    void done_ms();

private:
    int barWidth = 20;
    uint64_t total;
    uint64_t current;
    int last_prog = 0;
    std::chrono::_V2::system_clock::time_point start;
};

class Base64 {
public:
    static std::string decode(const std::string& str);
};
#endif