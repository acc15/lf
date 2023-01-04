#pragma once

#include "tree/tree.hpp"
#include "tree/tree_order.hpp"
#include "util/sort.hpp"
#include "io/format.hpp"

#include <string>
#include <vector>
#include <ostream>
#include <utility>
#include <concepts>
#include <algorithm>

namespace lf {

    template <tree_concept Tree, typename EntryOrder = void>
    struct tree_print {

        using tree_type = Tree;
        using map_type = typename tree_type::map_type;
        using entry_ptr = typename tree_type::entry_ptr;
        using queue_type = std::vector<entry_ptr>;
        using entry_sorter = sorter<EntryOrder>;

        static void add_entries(const map_type& entries, queue_type& queue) {
            queue.resize(queue.size() + entries.size());

            const auto begin = queue.rbegin();
            auto end = begin;
            for (const auto& entry: entries) {
                *end = &entry;
                ++end;
            }
            entry_sorter::sort(begin, end);
        }

        static std::ostream& print(std::ostream& s, const tree_type& node) {
            s << "<root> [" << with_cref_format<format::TREE>(node.data) << "]" << std::endl;

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

                s << p->first << " [" << with_cref_format<format::TREE>(p->second.data) << "]" << std::endl;
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

    template <tree_concept Tree>
    std::ostream& operator<<(std::ostream& s, const Tree& tree) {
        return tree_print<Tree>::print(s, tree);
    }

}