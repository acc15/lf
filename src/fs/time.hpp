#pragma once

#include <filesystem>

namespace lf {

    /** Returns truncated last_write_time for path, time must be truncated due to different file-systems has different timestamp precision */
    std::filesystem::file_time_type trunc_last_write_time(const std::filesystem::path& p);
    std::filesystem::file_time_type trunc_last_write_time(std::filesystem::file_time_type t);

}