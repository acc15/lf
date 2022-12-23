#include "index.hpp"

std::ostream& operator<<(std::ostream& s, const leafsync::index_flags& flags) {
    s << '[';
    switch (flags.mode) {
    case leafsync::index_sync_mode::SHALLOW:
        s << '.';
        break;

    case leafsync::index_sync_mode::RECURSIVE:
        s << '*';
        break;

    default:
        s << '-';
        break;
    }
    return s << (flags.sync ? '+' : '-') << ']';
}

struct tree_print_data {
    const leafsync::index_entry* entry;
    bool last;
};

void queue_add(const std::vector<leafsync::index_entry>& entries, std::vector<tree_print_data>& queue) {
    const auto b = entries.rbegin(), e = entries.rend();
    for (auto it = b; it != e; it++) {
        queue.emplace_back(tree_print_data{ .entry = &(*it), .last = it == b });
    }
}

std::ostream& operator<<(std::ostream& s, const leafsync::index_root& index) {
    s << "<root> " << index.flags << std::endl;

    std::vector<bool> indents;
    std::vector<tree_print_data> queue;
    queue_add(index.entries, queue);
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
        queue_add(e.entries, queue);
    }
    return s;
}
