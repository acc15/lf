#include "index_sync_mode.hpp"
#include "../util/pick.hpp"

namespace lf {
    const char* index_sync_mode_names[4] = { 
        "NONE", 
        "SHALLOW", 
        "RECURSIVE", 
        "(invalid value)" 
    };

    std::ostream& operator<<(std::ostream& s, const index_sync_mode& mode) {
        return s << pick_indexed_with_last_default(mode, index_sync_mode_names);
    }
}
