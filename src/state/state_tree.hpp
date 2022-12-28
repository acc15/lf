#pragma once

#include "tree/tree.hpp"
#include "io/format.hpp"

namespace lf {

    struct state_tree: tree<bool> {
        static const lf::format format = lf::format::BINARY;
        static const char* const name;
        static const char file_signature[4];
        static const uint8_t file_version;
    };

}