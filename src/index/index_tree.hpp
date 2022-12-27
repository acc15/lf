#pragma once

#include "../tree/tree.hpp"
#include "sync_mode.hpp"

namespace lf {
    
    using index_tree = tree<sync_mode>;

    struct index_desc {
        using type = index_tree;
        static const lf::format format = lf::format::BINARY;
        static const char* const name;
    };

}