#include "index_sync_mode.hpp"

namespace leafsync {
    const char* index_sync_mode_names[4] = { 
        "NONE", 
        "SHALLOW", 
        "RECURSIVE", 
        "(invalid value)" 
    };
}

std::ostream& operator<<(std::ostream& s, const leafsync::index_sync_mode& mode) {
    return s << leafsync::pick_indexed_with_last_default(mode, leafsync::index_sync_mode_names);
}