#pragma once

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "../io/with_format.hpp"

namespace leafsync {

    extern const char index_signature[];

    enum class index_sync_mode: std::uint8_t {
        NONE,
        SHALLOW,
        RECURSIVE
    };

    struct index_flags {
        index_sync_mode mode: 2;
        bool sync: 1;
    };

    static_assert(sizeof(index_flags) == 1, "sizeof(index_flags) must be exactly 1");

    struct index_entry;
    struct index_root {
        index_flags flags;
        std::vector<index_entry> entries;
    };

    struct index_entry {
        std::string name;
        index_flags flags;
        std::vector<index_entry> entries = {};
    };


    std::istream& read_index_fields(std::istream& s, leafsync::index_flags& flags, std::vector<index_entry>& entries);

}

std::ostream& operator<<(std::ostream& s, const leafsync::index_flags& flags);
std::ostream& operator<<(std::ostream& s, const leafsync::index_root& index);

std::ostream& operator<<(std::ostream& s, leafsync::with_format<leafsync::format::BINARY, const leafsync::index_flags> flags);
std::istream& operator>>(std::istream& s, leafsync::with_format<leafsync::format::BINARY, leafsync::index_flags> flags);

std::ostream& operator<<(std::ostream& s, leafsync::with_format<leafsync::format::BINARY, const leafsync::index_root> index);
std::istream& operator>>(std::istream& s, leafsync::with_format_and_errors<leafsync::format::BINARY, leafsync::index_root> index);

std::istream& operator>>(std::istream& s, leafsync::with_format<leafsync::format::BINARY, leafsync::index_entry> entry);
