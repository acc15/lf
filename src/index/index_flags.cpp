#include <algorithm>

#include "index_flags.hpp"
#include "../util/util.hpp"

using namespace leafsync;

namespace leafsync {

    const char mode_chars[] = { '-', '.', '*', '!' };
    const char sync_chars[] = { '-', '+'  };

}

std::ostream& operator<<(std::ostream& s, index_flags flags) {
    return s 
        << '['
        << pick_indexed_with_last_default(flags.mode, mode_chars)
        << pick_indexed(flags.sync, sync_chars)
        << ']';
}

std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_flags&> flags) {
    return s << reinterpret_cast<const char&>(flags.value);
}

std::istream& operator>>(std::istream& s, with_format<format::BINARY, index_flags&> flags) {
    return s >> reinterpret_cast<char&>(flags.value);
}
