#pragma once

#include "tree/tree.hpp"
#include "util/sort.hpp"

#include <vector>
#include <ostream>
#include <utility>
#include <concepts>
#include <algorithm>

namespace lf {

    template <tree_data T, typename Less = std::less<std::string>>
    struct tree_entry_name_order {
        using entry_ptr = typename tree<T>::entry_ptr;
        const Less less;
        bool operator()(entry_ptr l, entry_ptr r) {
            return less(l->first, r->first);
        }
    };

    template <tree_data T, typename Order = void>
    struct tree_print {

        using tree_type = tree<T>;
        using map_type = typename tree_type::map_type;
        using entry_ptr = typename tree_type::entry_ptr;
        using queue_type = std::vector<entry_ptr>;
        using entry_sorter = sorter<Order>;

        static void add_entries(const map_type& entries, queue_type& queue) {
            queue.reserve(queue.size() + entries.size());
            for (const auto& entry: entries) {
                queue.push_back(&entry);
            }
            entry_sorter::sort(queue.rbegin(), queue.rbegin() + entries.size());
        }

        static std::ostream& print(std::ostream& s, const tree_type& node) {
            s << "<root> [" << node.data << "]" << std::endl;

            std::vector<bool> indents;
            queue_type queue;
            add_entries(node.entries, queue);

            while (!queue.empty()) {
                entry_ptr p = queue.back();
                queue.pop_back();

                if (p == nullptr) {
                    indents.pop_back();
                    continue;
                }

                for (bool i : indents) {
                    s << (i ? "│   " : "    ");
                }
                
                bool last = queue.empty() || queue.back() == nullptr;
                s << (last ? "└── " : "├── ");

                s << p->first << " [" << p->second.data << "]" << std::endl;
                if (p->second.entries.empty()) {
                    continue;
                }

                indents.push_back(!last);
                queue.push_back(nullptr);
                add_entries(p->second.entries, queue);

            }
            return s;
        }

    };

    template <tree_data T>
    std::ostream& operator<<(std::ostream& s, const tree<T>& node) {
        return tree_print<T>::print(s, node);
    }

}