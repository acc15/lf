#pragma once

#include "../io/with_format.hpp"

#include "index_flags.hpp"
#include "index_entry.hpp"

namespace leafsync {

    extern const char index_signature[];

    struct index_root {
        index_flags flags;
        std::vector<index_entry> entries;
    };

}

std::ostream& operator<<(std::ostream& s, const leafsync::index_root& index);

std::ostream& operator<<(std::ostream& s, leafsync::with_format<leafsync::format::BINARY, const leafsync::index_root&> index);
std::istream& operator>>(std::istream& s, leafsync::with_format_and_errors<leafsync::format::BINARY, leafsync::index_root&> index);

