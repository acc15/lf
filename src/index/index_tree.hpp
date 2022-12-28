#pragma once

#include "tree/tree.hpp"
#include "index/sync_mode.hpp"

namespace lf {
    
    struct index_tree: tree<sync_mode> {
        static const char file_signature[4];
        static const uint8_t file_version;
    };

    struct index_desc {
        using type = index_tree;
        static const lf::format format = lf::format::BINARY;
        static const char* const name;
    };

}