#pragma once

#include <filesystem>

namespace lf {

    template <typename T>
    constexpr T integral_floor(T value, T num) {
        return num > 1 ? (value < 0 ? value - (num - 1) : value) / num * num : value;
    }

    /** Returns truncated last_write_time for path, time must be truncated due to different file-systems has different timestamp precision */
    std::filesystem::file_time_type ntfs_last_write_time(const std::filesystem::path& p);
    std::filesystem::file_time_type ntfs_last_write_time(std::filesystem::file_time_type t);

}