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
        using fs_period = fs_duration::period; // fs timepoint duration period - OS dependent (1nano - linux, 100nanos - windows)
        using ntfs_period = std::ratio_divide<std::micro, std::deca>; // NTFS timestamp precision - 100 nanoseconds (0.1 micro)

        constexpr intmax_t num = std::ratio_divide<ntfs_period, fs_period>::num;

#if defined(__linux__)
        static_assert(num == 100, "illegal ntfs/fs timepoint numerator");
#elif defined(_WIN32)
        static_assert(num == 1, "illegal ntfs/fs timepoint numerator");
#endif

        if constexpr (num > 1) {
            return fs_timepoint(fs_duration(integral_floor<fs_duration::rep>(t.time_since_epoch().count(), num)));
        } else {
            return t;
        }

    }

}