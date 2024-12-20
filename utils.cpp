#include "utils.h"
#include <boost/beast/core/detail/base64.hpp>

auto Base64::decode(const std::string& str) -> std::string {
    std::string dest;
    dest.resize(boost::beast::detail::base64::decoded_size(str.size()));
    auto const result = boost::beast::detail::base64::decode(
        &dest[0], str.data(), str.size());
    dest.resize(result.first);
    return dest;
}

Progress::Progress(uint64_t total) : total(total), current(0) {
    start = std::chrono::system_clock::now();
}
void Progress::prog(uint64_t current) {
    this->current = current;
    print();
}

void Progress::prog_delta(uint64_t delta) {
    current += delta;
    print();
}
void Progress::print() {
    if (static_cast<int>((current * 1.0 / total) * 100.0) == last_prog) return;
    last_prog = static_cast<int>((current * 1.0 / total) * 100.0);
    std::string s = "";
    int pos = barWidth * (current * 1.0 / total);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            s += "=";
        else if (i == pos)
            s += ">";
        else
            s += " ";
    }
    std::print("[{}]", s);
    //<< int((current * 1.0 / total) * 100.0) << " %";
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    //std::cout << current << " / " << total << " in " << duration.count() << "s, ";
    std::print("{}/{} in {}s", current, total, duration.count() / 1000.0);
    if (current > 0) {
        double itps = current * 1.0 / duration.count() * 1000;
        double eta = (total - current) / itps;
        std::print("({:.2f} it/s), ETA: {:.2f}s", itps, eta);
    }
    std::print("\r");
    std::flush(std::cout);
}
void Progress::done() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    used_microseconds = duration.count() * 1000;
    std::println("Finished in {:.2f}s!", duration.count() / 1000.0);
}
void Progress::done_ms() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
    used_microseconds = duration.count();
    std::println("Finished in {:.2f}ms!", duration.count() / 1000.0);
}