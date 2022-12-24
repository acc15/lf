#pragma once

#include <filesystem>

#include "../io/with_format.hpp"

#include "index_flags.hpp"
#include "index_entry.hpp"

namespace leafsync {

    extern const char index_signature[];

    struct index_root {
        index_flags flags;
        std::vector<index_entry> entries;

        // index_flags get(std::filesystem::path path);
        // void set(std::filesystem::path path, index_flags flags);
        // void remove(std::filesystem::path path);

    };

    std::ostream& operator<<(std::ostream& s, const index_root& index);
    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_root&> index);
    std::istream& operator>>(std::istream& s, with_format_and_errors<format::BINARY, index_root&> index);

}


