#pragma once

#include <cstdint>
#include <istream>
#include <ostream>

#include "index_sync_mode.hpp"
#include "../io/with_format.hpp"

namespace leafsync {

    struct index_flags {
        index_sync_mode mode: 2;
        bool sync: 1;
        std::uint8_t:5;
    };

    static_assert(sizeof(index_flags) == 1, "sizeof(index_flags) must be exactly 1");

}

std::ostream& operator<<(std::ostream& s, leafsync::index_flags flags);

std::ostream& operator<<(std::ostream& s, leafsync::with_format<leafsync::format::BINARY, const leafsync::index_flags&> flags);
std::istream& operator>>(std::istream& s, leafsync::with_format<leafsync::format::BINARY, leafsync::index_flags&> flags);
