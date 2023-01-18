#pragma once

#include <filesystem>

namespace lf {

    std::chrono::system_clock::time_point file_clock_to_system(const std::chrono::file_clock::time_point& v);

    /** Returns truncated last_write_time for path, time must be truncated due to different file-systems has different timestamp precision */
    std::filesystem::file_time_type ntfs_last_write_time(const std::filesystem::path& p);
    std::filesystem::file_time_type ntfs_last_write_time(std::filesystem::file_time_type t);

}