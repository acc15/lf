#include <cstdint>
#include <cstring>
#include <ios>
#include <istream>
#include <iterator>
#include <string>
#include <tuple>

#include "index.hpp"

namespace leafsync {

    const char index_signature[] = "LF";

    std::ostream& write_index_fields(std::ostream& s, const index_flags& flags, const std::vector<index_entry>& entries) {
        s << flags;
        for (const auto& e: entries) {
            s << e;
        }
        return s << '\0';
    }

    std::istream& read_index_fields(std::istream& s, leafsync::index_flags& flags, std::vector<index_entry>& entries) {
        s >> flags;
        if (!s.good()) {
            return s;
        }

        std::istream::int_type next;
        while ((next = s.peek()) != 0 && next != std::istream::traits_type::eof()) {
            auto& entry = entries.emplace_back();
            if (!(s >> entry)) {
                entries.pop_back();
                break;
            }
        }
        if (next == 0) {
            s.get();
        }
        return s;
    }

}

std::ostream& operator<<(std::ostream& s, const leafsync::index_flags& flags) {
    return s << reinterpret_cast<const char&>(flags);
}

std::istream& operator>>(std::istream& s, leafsync::index_flags& flags) {
    return s >> reinterpret_cast<char&>(flags);   
}

std::ostream& operator<<(std::ostream& s, const leafsync::index& index) {
    s << leafsync::index_signature << static_cast<uint8_t>(0);
    if (!s.good()) {
        return s;
    }
    return leafsync::write_index_fields(s, index.flags, index.entries);
}

std::istream& operator>>(std::istream& s, leafsync::index& index) {
    char signature[sizeof(leafsync::index_signature)];
    uint8_t version;
    
    if (!(s >> signature >> version)) {
        return s;
    }

    if (std::strcmp(signature, leafsync::index_signature) != 0) {
        s.clear(std::ios_base::failbit);
        return s;
    }

    if (version != 0) {
        s.clear(std::ios_base::failbit);
        return s;
    }

    return leafsync::read_index_fields(s, index.flags, index.entries);
}

std::ostream& operator<<(std::ostream& s, const leafsync::index_entry& entry) {
    if (!(s << entry.name << '\0')) {
        return s;
    }
    return leafsync::write_index_fields(s, entry.flags, entry.entries);
}

std::istream& operator>>(std::istream& s, leafsync::index_entry& entry) {
    if (!std::getline(s, entry.name, '\0')) {
        return s;
    }
    return leafsync::read_index_fields(s, entry.flags, entry.entries);
}