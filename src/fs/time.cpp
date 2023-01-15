#include "fs/time.hpp"

namespace lf {

    std::filesystem::file_time_type trunc_last_write_time(const std::filesystem::path& p) {
        return trunc_last_write_time(std::filesystem::last_write_time(p));
    }

    std::filesystem::file_time_type trunc_last_write_time(std::filesystem::file_time_type t) {
        using time_type = std::filesystem::file_time_type; 
        time_type::duration::rep r = t.time_since_epoch().count();
        r = (r < 0 ? r - 99 : r) / 100 * 100; 
        return time_type(time_type::duration(r));
    }

}