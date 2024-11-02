#ifndef UTILS_H
#define UTILS_H
#include <cstdint>
#include <iostream>

class Progress {
public:
    Progress(uint64_t total) : total(total), current(0) {}
    void prog(uint64_t current) {
        this->current = current;
    }
    void prog_delta(uint64_t delta) {
        current += delta;
        print();
    }
    void print() {
        if (int((current * 1.0 / total) * 100.0) == last_prog) return;
        last_prog = int((current * 1.0 / total) * 100.0);
        std::cout << "[";
        int pos = barWidth * (current * 1.0 / total);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)
                std::cout << "=";
            else if (i == pos)
                std::cout << ">";
            else
                std::cout << " ";
        }
        std::cout << "] " << int((current * 1.0 / total) * 100.0) << " %\r";
        std::cout.flush();
    }

private:
    int barWidth = 20;
    uint64_t total;
    uint64_t current;
    int last_prog = 0;
};
#endif