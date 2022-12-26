#pragma once

#include <algorithm>
#include <istream>
#include <ostream>
#include <vector>

#include "tree.hpp"
#include "../io/with_format.hpp"

namespace lf {

    extern const char* const tree_signature;
    extern const uint8_t tree_version;

    template <typename T>
    struct tree_binary {

        using tree_type = tree<T>;
        using entry_map = typename tree_type::entry_map;
        using entry_map_ptr = typename entry_map::const_pointer;
        using entry_map_ref = typename entry_map::const_reference;

        static void add_entries(const entry_map& entries, std::vector<entry_map_ptr>& queue) {
            std::for_each(entries.begin(), entries.end(), [&queue](entry_map_ref e) { queue.push_back(&e); });
        }

        static std::ostream& write(std::ostream& s, const tree_type& tree) {
            s << tree_signature << tree_version;
            if (!s.good()) {
                return s;
            }

            s << with_ref_format<format::BINARY>(tree.data);

            std::vector<entry_map_ptr> queue;
            add_entries(tree.value.entries, queue);

            while (!queue.empty()) {
                const auto e = queue.back();
                queue.pop_back();

                if (e == nullptr) {
                    s << '\0';
                    continue;
                }

                s << e->first << '\0' << with_ref_format<format::BINARY>(e->second.data);
                if (e->second.entries.empty()) {
                    if (std::all_of(queue.begin(), queue.end(), [](auto p) { return p == nullptr; })) {
                        break;
                    }
                    s << '\0';
                    continue;
                }

                queue.push_back(nullptr);
                add_entries(e->second.entries, queue);

            }
            return s;
        }

        static std::istream& read(std::istream& s, tree_type& tree, errors& err) {
            return s;
        }

    };

    template <typename T>
    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const tree<T>&> tree) {
        return tree_binary<T>::write(s, tree);
    }
    
    template <typename T>
    std::istream& operator>>(std::istream& s, with_format_and_errors<format::BINARY, tree<T>&> tree) {
        return tree_binary<T>::read(s, tree.value, tree.err);
    }

}