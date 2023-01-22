#include "log/log_level.hpp"

#include "util/pick.hpp"

namespace lf {

    const char* log_level_names[] = {
        "TRACE", 
        "DEBUG", 
        "INFO", 
        "WARN", 
        "ERROR", 
        "UNKNOWN" 
    };

    std::ostream& operator<<(std::ostream& s, const log_level& l) {
        return s << pick_indexed_with_last_default(l, log_level_names);
    }

}