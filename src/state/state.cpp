#include "state/state.hpp"

namespace lf {

    const char* const state::name = "state";
    const char state::file_signature[4] = "LFS";
    const uint8_t state::file_version = 0;

    state::state(data_type data, const map_type& map): tree { .data = data, .entries = map } {}

    std::ostream& operator<<(std::ostream& s, const with_format<tree_print_format, const bool> v) {
        return s.put(v.value ? '+' : ' ');
    }

    std::ostream& operator<<(std::ostream& s, const with_format<tree_binary_format, const bool> v) {
        return s.put(static_cast<std::ostream::char_type>(v.value));
    }

    std::istream& operator>>(std::istream& s, with_format<tree_binary_format, bool> v) {
        std::istream::char_type c;
        if (s >> c) {
            v.value = static_cast<bool>(c);
        }
        return s;
    }

}