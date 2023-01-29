#include "index/sync_mode.hpp"
#include "util/pick.hpp"

namespace lf {
    
    const char* sync_mode_names[] = { 
        "IGNORE", 
        "SHALLOW", 
        "RECURSIVE", 
        "(invalid sync mode)" 
    };

    const char* sync_mode_signs[] = { 
        "-", 
        "+", 
        "*", 
        "#" 
    };

    std::ostream& operator<<(std::ostream& s, const sync_mode& mode) {
        return s << pick_indexed(mode, sync_mode_names);
    }

    std::ostream& operator<<(std::ostream& s, with_format<format::TREE, const sync_mode&> mode) {
        return s << pick_indexed(mode.value, sync_mode_signs);
    }

    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const sync_mode&> mode) {
        return s << static_cast<std::uint8_t>(mode.value);
    }

    std::istream& operator>>(std::istream& s, with_format<format::BINARY, sync_mode&> mode) {
        std::uint8_t value;
        if (s >> value) {
            mode.value = static_cast<sync_mode>(value);
        }
        return s;
    }

}
