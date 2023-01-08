#pragma once

#include "tree/tree.hpp"
#include "index/sync_mode.hpp"

namespace lf {
    
    struct index_tree: tree<sync_mode> {
        using type = index_tree;
        static const lf::format format = lf::format::BINARY;
        static const bool binary = true;
        static const char* const name;
        static const char file_signature[4];
        static const uint8_t file_version;

        index_tree(data_type data = {}, const map_type& map = {});
    };

}