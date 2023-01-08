#pragma once

#include <ostream>
#include <chrono>
#include <string>

namespace lf {

    struct format_date_time {
        std::chrono::system_clock::time_point tp;
        std::string format = "%FT%T";
        bool utc = false;
    };

    std::ostream& operator<<(std::ostream& s, const format_date_time& fmt);

}