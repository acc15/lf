#pragma once

#include <iostream>

#include "io/format.hpp"
#include "tree/binary.hpp"
#include "tree/print.hpp"

namespace lf {
    
    enum class sync_mode {
        IGNORE,
        SHALLOW,
        RECURSIVE
    };

    std::ostream& operator<<(std::ostream& s, const sync_mode& mode);
    std::ostream& operator<<(std::ostream& s, with_format<tree_print_format, const sync_mode> mode);
    std::ostream& operator<<(std::ostream& s, with_format<tree_binary_format, const sync_mode> mode);
    std::istream& operator>>(std::istream& s, with_format<tree_binary_format, sync_mode> mode);

}

