#pragma once

#include <istream>
#include <ostream>
#include <string>
#include <vector>

namespace leafsync {

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
    struct index {
        index_flags flags;
        std::vector<index_entry> entries;
    };

    struct index_entry: index {
        std::string name;
    };

}

std::ostream& operator<<(std::ostream& s, const leafsync::index_flags& flags);
std::istream& operator>>(std::istream& s, leafsync::index_flags& flags);

std::ostream& operator<<(std::ostream& s, const leafsync::index& index);
std::istream& operator>>(std::istream& s, leafsync::index& index);

std::ostream& operator<<(std::ostream& s, const leafsync::index_entry& entry);
std::istream& operator>>(std::istream& s, leafsync::index_entry& entry);
