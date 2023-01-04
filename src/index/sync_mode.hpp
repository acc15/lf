#pragma once

#include <iostream>

#include "io/format.hpp"

namespace lf {
    
    enum class sync_mode {
        UNSPECIFIED,
        SHALLOW,
        RECURSIVE,
        IGNORE
    };

    std::ostream& operator<<(std::ostream& s, const sync_mode& mode);
    std::ostream& operator<<(std::ostream& s, with_format<format::TREE, const sync_mode&> mode);
    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const sync_mode&> mode);
    std::istream& operator>>(std::istream& s, with_format<format::BINARY, sync_mode&> mode);

}

