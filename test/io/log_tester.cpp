#include "io/log_tester.hpp"

namespace lf {

    log_tester::log_tester(log::level level): original_min_level(log.min_level) {
        log.min_level = level;
        log.stream = &out;
    }

    log_tester::~log_tester() {
        log.min_level = original_min_level;
        log.stream = nullptr;
    }

    std::string log_tester::str() const {
        return out.str();
    }

    std::vector<std::string> log_tester::lines() {
        out.clear();
        out.seekg(0);
        
        std::vector<std::string> lines;
        
        std::string line;
        while (std::getline(out, line)) {
            lines.emplace_back(std::move(line));
        }
        return lines;
    }

}