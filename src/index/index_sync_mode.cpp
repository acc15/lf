#include "index_sync_mode.hpp"

namespace leafsync {
    const char* index_sync_mode_names[4] = { 
        "NONE", 
        "SHALLOW", 
        "RECURSIVE", 
        "(invalid value)" 
    };
}