#pragma once

#include <vector>
#include <ostream>
#include <utility>

#include "tree.hpp"

namespace lf {

    template <typename T>
    struct tree_print {

        using node_type = tree<T>;
        using entry_map = typename node_type::entry_map;
        using queue_type = std::vector<tree_print>;

        typename entry_map::const_pointer entry;
        bool last;

        static void add_entries(const entry_map& entries, queue_type& queue) {
            if (entries.empty()) {
                return;
            }

            const size_t first_index = queue.size();
            queue.resize(first_index + entries.size());
            queue[first_index].last = true;

            size_t index = queue.size() - 1;
            for (const auto& entry: entries) {
                queue[index].entry = &entry;
                --index;
            }
        }

        static std::ostream& print(std::ostream& s, const node_type& node) {
            s << "<root> [" << node.data << "]" << std::endl;

            std::vector<bool> indents;
            queue_type queue;
            add_entries(node.entries, queue);
            while (!queue.empty()) {
                const auto p = queue.back();
                queue.pop_back();

                if (p.entry == nullptr) {
                    indents.pop_back();
                    continue;
                }

                for (bool i : indents) {
                    s << (i ? "│   " : "    ");
                }
                
                s << (p.last ? "└── " : "├── ");

                const auto& e = *p.entry;
                s << e.first << " [" << e.second.data << "]" << std::endl;

                if (e.second.entries.empty()) {
                    continue;
                }

                indents.push_back(!p.last);
                queue.emplace_back(tree_print { .entry = nullptr, .last = false });
                add_entries(e.second.entries, queue);
            }
            return s;
        }

    };

    template <typename T>
    std::ostream& operator<<(std::ostream& s, const tree<T>& node) {
        return tree_print<T>::print(s, node);
    }

}