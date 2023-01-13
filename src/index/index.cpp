#include "index/index.hpp"

namespace lf {

    const char index::file_signature[4] = "LFI";
    const uint8_t index::file_version = 0;
    const char* const index::name = "index";

    index::index(data_type data, const map_type& map): tree { .data = data, .entries = map } {}

}