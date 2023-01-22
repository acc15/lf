#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "log/log_level.hpp"

namespace lf {

    struct log_tester {
        log_tester(log_level level);
        ~log_tester();
        
        std::string str() const;
        bool contains(std::string_view v) const;

        std::stringstream out;
    };

}