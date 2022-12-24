#include <algorithm>
#include <fmt/core.h>
#include <string>

#include "index_root.hpp"

using namespace std;

namespace lf {

    const char index_signature[] = "LF";

    void rev_add(const vector<index_entry>& entries, vector<const index_entry*>& queue) {
        for_each(entries.rbegin(), entries.rend(), [&queue](const index_entry& e) { queue.push_back(&e); });
    }

    ostream& operator<<(ostream& s, with_format<format::BINARY, const index_root&> index) {
        s << index_signature << static_cast<uint8_t>(0);
        if (!s.good()) {
            return s;
        }

        s << with_ref_format<format::BINARY>(index.value.flags);

        vector<const index_entry*> queue;
        rev_add(index.value.entries, queue);
        while (!queue.empty()) {
            const auto e = queue.back();
            queue.pop_back();

            if (e == nullptr) {
                s << '\0';
                continue;
            }
            s << e->name << '\0' << with_ref_format<format::BINARY>(e->flags);
            if (e->entries.empty()) {
                if (std::all_of(queue.begin(), queue.end(), [](auto p) { return p == nullptr; })) {
                break;
                }
                s << '\0';
                continue;
            }

            queue.push_back(nullptr);
            rev_add(e->entries, queue);

        }
        return s;
    }

    istream& operator>>(istream& s, with_format_and_errors<format::BINARY, index_root&> index) {
        char signature[sizeof(index_signature)];
        uint8_t version;

        if (!(s >> signature >> version)) {
            return s;
        }

        if (strcmp(signature, index_signature) != 0) {
            index.err(fmt::format("invalid file signature: {}", signature));
            s.clear(ios_base::failbit);
            return s;
        }

        if (version != 0) {
            index.err(fmt::format("invalid file version: {}", static_cast<unsigned int>(version)));
            s.clear(ios_base::failbit);
            return s;
        }

        if (!(s >> with_ref_format<format::BINARY>(index.value.flags))) {
            return s;
        }

        vector<vector<index_entry>*> stack;
        stack.push_back(&index.value.entries);

        do {

            istream::int_type next = s.peek();
            if (next == istream::traits_type::eof()) {
                break;
            }

            if (next == 0) {
                s.get();
                stack.pop_back();
                continue;
            }
            
            index_entry& entry = stack.back()->emplace_back();
            if (!getline(s, entry.name, '\0')) {
                break;
            }

            if (!(s >> with_ref_format<format::BINARY>(entry.flags))) {
                break;
            }

            stack.push_back(&entry.entries);

        } while (!stack.empty());

        return s;
    }

}
