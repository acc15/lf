#include <cstring>
#include <vector>
#include <algorithm>

#include "index_entry.hpp"

namespace lf {

    const char index_signature[] = "LF";
    const uint8_t index_version = 0;

    void queue_entries(const index_entry::entry_map& entries, std::vector<index_entry::entry_map::const_pointer>& queue) {
        std::for_each(entries.begin(), entries.end(), [&queue](index_entry::entry_map::const_reference e) { queue.push_back(&e); });
    }

    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_entry&> index) {
        s << index_signature << index_version;
        if (!s.good()) {
            return s;
        }

        s << with_ref_format<format::BINARY>(index.value.flags);

        std::vector<index_entry::entry_map::const_pointer> queue;
        queue_entries(index.value.entries, queue);
        while (!queue.empty()) {
            const auto e = queue.back();
            queue.pop_back();

            if (e == nullptr) {
                s << '\0';
                continue;
            }

            s << e->first << '\0' << with_ref_format<format::BINARY>(e->second.flags);
            if (e->second.entries.empty()) {
                if (std::all_of(queue.begin(), queue.end(), [](auto p) { return p == nullptr; })) {
                    break;
                }
                s << '\0';
                continue;
            }

            queue.push_back(nullptr);
            queue_entries(e->second.entries, queue);

        }
        return s;
    }

    std::istream& operator>>(std::istream& s, with_format_and_errors<format::BINARY, index_entry&> index) {
        char signature[sizeof(index_signature)];
        uint8_t version;

        if (!(s >> signature >> version)) {
            return s;
        }

        if (std::strcmp(signature, index_signature) != 0) {
            index.err << "invalid file signature: " << signature << errors::end;
            s.clear(std::ios_base::failbit);
            return s;
        }

        if (version != 0) {
            index.err << "invalid file version: " << static_cast<unsigned int>(version) << errors::end;
            s.clear(std::ios_base::failbit);
            return s;
        }

        if (!(s >> with_ref_format<format::BINARY>(index.value.flags))) {
            return s;
        }

        std::vector<index_entry::entry_map*> stack;
        stack.push_back(&index.value.entries);

        do {

            std::istream::int_type next = s.peek();
            if (next == std::istream::traits_type::eof()) {
                break;
            }

            if (next == 0) {
                s.get();
                stack.pop_back();
                continue;
            }
            
            std::string name;
            if (!getline(s, name, '\0')) {
                break;
            }

            index_entry& entry = (*stack.back())[name];
            if (!(s >> with_ref_format<format::BINARY>(entry.flags))) {
                break;
            }

            stack.push_back(&entry.entries);

        } while (!stack.empty());

        return s;
    }

}
