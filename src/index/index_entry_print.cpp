#include <vector>

#include "index_entry.hpp"

using namespace std;

namespace lf {

    struct tree_print_data {
        index_entry::entry_map::const_pointer entry = nullptr;
        bool last = false;
    };

    void queue_entries(const index_entry::entry_map& entries, vector<tree_print_data>& queue) {
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

    ostream& operator<<(ostream& s, const index_entry& index) {
        s << "<root> " << index.flags << endl;

        vector<bool> indents;
        vector<tree_print_data> queue;
        queue_entries(index.entries, queue);
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
            s << e.first << " " << e.second.flags << endl;

            if (e.second.entries.empty()) {
                continue;
            }

            indents.push_back(!p.last);
            queue.emplace_back(tree_print_data{ .entry = nullptr, .last = false });
            queue_entries(e.second.entries, queue);
        }
        return s;
    }

}

