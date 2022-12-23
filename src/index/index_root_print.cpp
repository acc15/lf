#include "index_root.hpp"

using namespace leafsync;

struct tree_print_data {
    const index_entry* entry;
    bool last;
};

void rev_add(const std::vector<index_entry>& entries, std::vector<tree_print_data>& queue) {
    const auto b = entries.rbegin(), e = entries.rend();
    for (auto it = b; it != e; it++) {
        queue.emplace_back(tree_print_data{ .entry = &(*it), .last = it == b });
    }
}

std::ostream& operator<<(std::ostream& s, const index_root& index) {
    s << "<root> " << index.flags << std::endl;

    std::vector<bool> indents;
    std::vector<tree_print_data> queue;
    rev_add(index.entries, queue);
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
        s << e.name << " " << e.flags << std::endl;

        if (e.entries.empty()) {
            continue;
        }

        indents.push_back(!p.last);
        queue.emplace_back(tree_print_data{ .entry = nullptr, .last = false });
        rev_add(e.entries, queue);
    }
    return s;
}
