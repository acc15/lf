#pragma once

#include <cstdint>
#include <ostream>

namespace leafsync {
    
    enum class index_sync_mode: std::uint8_t {
        NONE,
        SHALLOW,
        RECURSIVE
    };

    extern const char* index_sync_mode_names[4];
    
    std::ostream& operator<<(std::ostream& s, const index_sync_mode& mode);

}

