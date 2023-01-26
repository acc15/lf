#pragma once

#include <ostream>
#include <chrono>
#include <string_view>
#include <ctime>
#include <filesystem>

namespace lf {

    template <typename Timepoint>
    struct format_date_time_s {
        Timepoint tp;
        bool utc;
        std::string_view format;
    };

    template <typename Timepoint>
    format_date_time_s<Timepoint> format_date_time(const Timepoint& tp, bool utc = false, std::string_view format = "%FT%T.%N03") {
        return format_date_time_s<Timepoint> { tp, utc, format };
    }

    void time_to_tm(bool utc, const std::time_t& t, std::tm& tm);

    std::ostream& operator<<(std::ostream& s, const format_date_time_s<std::chrono::system_clock::time_point>& fmt);
    std::ostream& operator<<(std::ostream& s, const format_date_time_s<std::chrono::file_clock::time_point>& fmt);

}