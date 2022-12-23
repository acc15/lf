#pragma once

#include <cstdint>
#include <ostream>

#include "../util/util.hpp"

namespace leafsync {
    
    enum class index_sync_mode: std::uint8_t {
        NONE,
        SHALLOW,
        RECURSIVE
    };

    extern const char* index_sync_mode_names[4];

}

template <typename Stream>
Stream& operator<<(Stream& s, const leafsync::index_sync_mode& mode) {
    return s << leafsync::pick_indexed_with_last_default(mode, leafsync::index_sync_mode_names);
}
