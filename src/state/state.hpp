#pragma once

#include <ostream>

#include "tree/tree.hpp"
#include "tree/binary.hpp"
#include "tree/print.hpp"
#include "tree/tracked_tree.hpp"

namespace lf {

    struct state: tree<bool> {
        using format = tree_binary_format;
        static const char* const name;
        static const char file_signature[4];
        static const uint8_t file_version;

        state(data_type data = {}, const map_type& map = {});
    };

    using tracked_state = tracked_tree<state>;

    std::ostream& operator<<(std::ostream& s, with_format<tree_print_format, const bool> v);
    std::ostream& operator<<(std::ostream& s, with_format<tree_binary_format, const bool> v);
    std::istream& operator>>(std::istream& s, with_format<tree_binary_format, bool> v);

}