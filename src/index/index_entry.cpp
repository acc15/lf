#include "index_entry.hpp"
#include "index_flags.hpp"
#include <iterator>
#include <optional>

namespace lf {

    index_flags index_entry::get(const std::filesystem::path& path) const {
        const index_entry* e = this;
        for (const auto& el: path) {
            const auto it = e->entries.find(el.string());
            if (it == e->entries.end()) {
                return index_flags();
            }
            e = &it->second;
        }
        return e->flags;
    }
    
    void index_entry::set(const std::filesystem::path& path, index_flags flags) {
        
    }

}