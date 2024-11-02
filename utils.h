#ifndef UTILS_H
#define UTILS_H
#include <cstdint>
#include <iostream>
#include <format>
#include <chrono>

class Progress {
public:
    Progress(uint64_t total) : total(total), current(0) {
        start = std::chrono::system_clock::now();
    }
    void prog(uint64_t current) {
        this->current = current;
        print();
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
        std::cout << "] ";
        //<< int((current * 1.0 / total) * 100.0) << " %";
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        //std::cout << current << " / " << total << " in " << duration.count() << "s, ";
        std::cout << std::format("{}/{} in {}s", current, total, duration.count() / 1000.0);
        if (current > 0) {
            double itps = current * 1.0 / duration.count() * 1000;
            double eta = (total - current) / itps;
            std::cout << std::format("({:.2f} it/s), ETA: {:.2f}s", itps, eta);
        }
        std::cout << "\r";
        std::cout.flush();
    }
    void done() {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        std::cout << std::format("Finished in {:.2f}s!", duration.count() / 1000.0) << std::endl;
    }

private:
    int barWidth = 20;
    uint64_t total;
    uint64_t current;
    int last_prog = 0;
    std::chrono::_V2::system_clock::time_point start;
};
#endif