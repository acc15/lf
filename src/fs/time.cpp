#include "fs/time.hpp"

namespace lf {

    std::chrono::system_clock::time_point file_clock_to_system(const std::chrono::file_clock::time_point& v) {
        // TODO: std::chrono::clock_cast not implemented in GNU C++ library (however it provides file_clock::to_sys)
        // After implementation of clock_cast in GNU lib - this #ifdef should be replaced with clock_cast
#ifdef __GNUG__
        return std::chrono::file_clock::to_sys(v);
#else
        return std::chrono::clock_cast<std::chrono::system_clock>(v);
#endif
    }

    std::filesystem::file_time_type ntfs_last_write_time(const std::filesystem::path& p) {
        return ntfs_last_write_time(std::filesystem::last_write_time(p));
    }

    std::filesystem::file_time_type ntfs_last_write_time(std::filesystem::file_time_type t) {

        using fs_timepoint = std::filesystem::file_time_type; 
        using fs_duration = fs_timepoint::duration;
        using fs_period = fs_duration::period;

        using ntfs_period = std::ratio_multiply<std::nano, std::hecto>;
        using ntfs_duration = std::chrono::duration<fs_duration::rep, ntfs_period>;

        constexpr intmax_t num = std::ratio_divide<ntfs_period, fs_period>::num;

#if defined(__linux__)
        static_assert(num == 100, "wrong ntfs/fs timepoint numerator");
#elif defined(_WIN32)
        static_assert(num == 1, "wrong ntfs/fs timepoint numerator");
#endif
        if constexpr (num > 1) {
            const ntfs_duration nd = std::chrono::floor<ntfs_duration>(t.time_since_epoch());
            const fs_duration fd = std::chrono::duration_cast<fs_duration>(nd);
            return fs_timepoint(fd);
        } else {
            return t;
        }

    }

}