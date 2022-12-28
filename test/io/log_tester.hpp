#pragma once

#include <vector>
#include <string>
#include <sstream>
#include "io/log.hpp"

namespace lf {

    struct log_tester {
        log_tester(log::level level = log::default_level);
        ~log_tester();
        
        std::string str() const;
        std::vector<std::string> lines();
        bool contains(std::string_view v) const;

        std::stringstream out;
        log::level original_min_level;
    };

}