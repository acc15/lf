#include <algorithm>
#include <fmt/core.h>

#include "index.hpp"

using namespace leafsync;

namespace leafsync {

    const char index_signature[] = "LF";

    std::istream& read_index_fields(std::istream& s, leafsync::index_flags& flags, std::vector<index_entry>& entries) {
        s >> formatted_as<format::BINARY>(flags);
        if (!s.good()) {
            return s;
        }

        std::istream::int_type next;
        while ((next = s.peek()) != 0 && next != std::istream::traits_type::eof()) {
            auto& entry = entries.emplace_back();
            if (!(s >> formatted_as<format::BINARY>(entry))) {
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

void reverse_add(const std::vector<index_entry>& entries, std::vector<const index_entry*>& queue) {
    std::for_each(entries.rbegin(), entries.rend(), [&queue](const auto& e) { queue.push_back(&e); });
}

std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_flags> flags) {
    return s << reinterpret_cast<const char&>(flags.value);
}

std::istream& operator>>(std::istream& s, with_format<format::BINARY, index_flags> flags) {
    return s >> reinterpret_cast<char&>(flags.value);
}

std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_root> index) {
    s << index_signature << static_cast<uint8_t>(0);
    if (!s.good()) {
        return s;
    }

    s << formatted_as<format::BINARY>(index.value.flags);

    std::vector<const index_entry*> queue;
    reverse_add(index.value.entries, queue);
    while (!queue.empty()) {
        const auto e = queue.back();
        queue.pop_back();

        if (e == nullptr) {
            s << '\0';
            continue;
        }
        s << e->name << '\0' << formatted_as<format::BINARY>(e->flags);
        if (e->entries.empty()) {
            //if (std::all_of(queue.begin(), queue.end(), [](auto p) { return p == nullptr; })) {
            //    break;
            //}
            s << '\0';
            continue;
        }

        queue.push_back(nullptr);
        reverse_add(e->entries, queue);
    }
    // s << '\0';

    return s;
}

std::istream& operator>>(std::istream& s, with_format_and_errors<format::BINARY, index_root> index) {
    char signature[sizeof(leafsync::index_signature)];
    uint8_t version;

    if (!(s >> signature >> version)) {
        return s;
    }

    if (std::strcmp(signature, leafsync::index_signature) != 0) {
        index.err(fmt::format("invalid file signature: {}", signature));
        s.clear(std::ios_base::failbit);
        return s;
    }

    if (version != 0) {
        index.err(fmt::format("invalid file version: {}", static_cast<unsigned int>(version)));
        s.clear(std::ios_base::failbit);
        return s;
    }



    return leafsync::read_index_fields(s, index.value.flags, index.value.entries);
}

std::istream& operator>>(std::istream& s, with_format<format::BINARY, index_entry> entry) {
    if (!std::getline(s, entry.value.name, '\0')) {
        return s;
    }
    return leafsync::read_index_fields(s, entry.value.flags, entry.value.entries);
}
