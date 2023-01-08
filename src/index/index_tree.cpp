#include "index/index_tree.hpp"

namespace lf {

    const char index_tree::file_signature[4] = "LFI";
    const uint8_t index_tree::file_version = 0;
    const char* const index_tree::name = "index";

    index_tree::index_tree(data_type data, const map_type& map): tree { .data = data, .entries = map } {}

}