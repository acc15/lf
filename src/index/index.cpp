#include "index.hpp"
#include <cstdint>

std::ostream& operator<<(std::ostream& s, const leafsync::index_flags& flags) {
    return s << reinterpret_cast<const char&>(flags);   
}

std::istream& operator>>(std::istream& s, leafsync::index_flags& flags) {
    return s >> reinterpret_cast<char&>(flags);   
}