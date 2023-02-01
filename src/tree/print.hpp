#pragma once

#include "tree/tree.hpp"
#include "io/format.hpp"

#include <vector>
#include <ostream>

namespace lf {

    struct tree_print_format: format<false> {};

    template <tree_type Tree>
    struct tree_print {

        using tree_type = Tree;
        using map_type = typename tree_type::map_type;
        using entry_ptr = typename tree_type::entry_ptr;
        using queue_type = std::vector<entry_ptr>;

        static void add_entries(const map_type& entries, queue_type& queue) {
            queue.resize(queue.size() + entries.size());

            const auto begin = queue.rbegin();
            auto end = begin;
            for (const auto& entry: entries) {
                *end = &entry;
                ++end;
            }
        }

        static std::ostream& print(std::ostream& s, const tree_type& node) {
            s << "<root> [" << write_as<tree_print_format>(node.data) << "]" << std::endl;

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

                s << p->first << " [" << write_as<tree_print_format>(p->second.data) << "]" << std::endl;
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

    template <tree_type Tree>
    std::ostream& operator<<(std::ostream& s, const Tree& tree) {
        return tree_print<Tree>::print(s, tree);
    }

}