#pragma once

#include "tree/tree.hpp"
#include "tree/tracked_tree.hpp"
#include "index/sync_mode.hpp"

namespace lf {
    
    struct index: tree<sync_mode> {
        using type = index;
        static const lf::format format = lf::format::BINARY;
        static const bool binary = true;
        static const char* const name;
        static const char file_signature[4];
        static const uint8_t file_version;

        index(data_type data = {}, const map_type& map = {});
    };

    using tracked_index = tracked_tree<index>;

}