#pragma once

#include <ostream>

#include "tree/tree.hpp"
#include "tree/tracked_tree.hpp"
#include "io/format.hpp"

namespace lf {

    struct state: tree<bool> {
        static const lf::format format = lf::format::BINARY;
        static const bool binary = true;
        static const char* const name;
        static const char file_signature[4];
        static const uint8_t file_version;

        state(data_type data = {}, const map_type& map = {});
    };

    using tracked_state = tracked_tree<state>;

    std::ostream& operator<<(std::ostream& s, with_format<format::TREE, const bool&> data);
    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const bool&> data);
    std::istream& operator>>(std::istream& s, with_format<format::BINARY, bool&> data);

}