#pragma once

#include "log/log_level.hpp"

#include <chrono>
#include <string>

namespace lf {

    struct log_message {
        using log_timestamp = std::chrono::system_clock::time_point;
        
        log_level       level;
        log_timestamp   timestamp;
        std::string     text = "";
    };

}