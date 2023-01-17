#include "fs/time.hpp"

namespace lf {

    std::filesystem::file_time_type ntfs_last_write_time(const std::filesystem::path& p) {
        return ntfs_last_write_time(std::filesystem::last_write_time(p));
    }

    std::filesystem::file_time_type ntfs_last_write_time(std::filesystem::file_time_type t) {

        using fs_timepoint = std::filesystem::file_time_type; 
        using fs_duration = fs_timepoint::duration;
        using fs_period = fs_duration::period;
        using fs_rep = fs_duration::rep;
        using ntfs_period = std::ratio_divide<std::micro, std::deca>;

        constexpr intmax_t num = std::ratio_divide<ntfs_period, fs_period>::num;
        if constexpr (num > 1) {
            return fs_timepoint(fs_duration(integral_floor<fs_rep, num>(t.time_since_epoch().count())));
        } else {
            return t;
        }

    }

}