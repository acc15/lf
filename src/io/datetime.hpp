#pragma once

#include <ostream>
#include <chrono>
#include <string>
#include <ctime>

namespace lf {

    struct format_date_time {
        std::chrono::system_clock::time_point tp;
        std::string format = "%FT%T";
        bool utc = false;
    };

    void time_to_tm(bool utc, const std::time_t& t, std::tm& tm);
    std::ostream& operator<<(std::ostream& s, const format_date_time& fmt);

}