#pragma once

#include <cstdint>
#include <istream>
#include <ostream>

#include "index_sync_mode.hpp"
#include "../io/with_format.hpp"

namespace lf {

    struct index_flags {
        index_sync_mode mode: 2 = index_sync_mode::NONE;
        bool sync: 1 = false;
        std::uint8_t: 5;
    };

    static_assert(sizeof(index_flags) == 1, "sizeof(index_flags) must be exactly 1");

    std::ostream& operator<<(std::ostream& s, const index_flags& flags);

    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_flags&> flags);
    std::istream& operator>>(std::istream& s, with_format<format::BINARY, index_flags&> flags);

}

