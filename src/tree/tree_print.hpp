#pragma once

#include "tree/tree.hpp"

#include <vector>
#include <ostream>
#include <utility>

namespace lf {

    template <tree_data T, template <typename, typename> typename Map>
    struct tree_print {

        using tree_type = tree<T, Map>;
        using map_type = typename tree_type::map_type;
        using queue_type = std::vector<tree_print>;

        typename map_type::const_pointer entry;
        bool last;

        static void add_entries(const map_type& entries, queue_type& queue) {
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

        static std::ostream& print(std::ostream& s, const tree_type& node) {
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

    template <tree_data T, template <typename, typename> typename Map>
    std::ostream& operator<<(std::ostream& s, const tree<T, Map>& node) {
        return tree_print<T, Map>::print(s, node);
    }

}