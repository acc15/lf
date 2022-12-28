#pragma once

#include <iostream>

#include "io/format.hpp"

namespace lf {
    
    enum class sync_mode {
        NONE,
        SHALLOW,
        RECURSIVE
    };

    extern const char* sync_mode_names[4];
    
    std::ostream& operator<<(std::ostream& s, const sync_mode& mode);
    std::ostream& operator<<(std::ostream& s, const with_format<format::BINARY, const sync_mode&> mode);
    std::istream& operator>>(std::istream& s, const with_format<format::BINARY, sync_mode&> mode);

}

