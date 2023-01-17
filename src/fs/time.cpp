#include "fs/time.hpp"

namespace lf {

    std::filesystem::file_time_type ntfs_last_write_time(const std::filesystem::path& p) {
        return ntfs_last_write_time(std::filesystem::last_write_time(p));
    }

    std::filesystem::file_time_type ntfs_last_write_time(std::filesystem::file_time_type t) {
#if defined _MSC_VER
        // MSVC already uses file_time_type which is fully compatible with NTFS timestampts
        return t;
#else
        using fs_timepoint = std::filesystem::file_time_type; 
        using fs_duration = fs_timepoint::duration;
        using fs_rep = fs_duration::rep;

        fs_rep d = t.time_since_epoch().count();
        fs_duration nd = fs_duration((d < 0 ? d - 99L : d) / 100 * 100);
        return fs_timepoint(nd);
#endif
    }

}