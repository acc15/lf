#include <filesystem>

#include "index_entry.hpp"
#include "index_sync_mode.hpp"

using namespace std::filesystem;

namespace lf {

    index_entry* index_entry::entry(const path& path) {
        index_entry* e = this;
        for (const auto& el: path) {
            const auto it = e->entries.find(el.string());
            if (it == e->entries.end()) {
                return nullptr;
            }
            e = &it->second;
        }
        return e;
    }

    const index_entry* index_entry::entry(const path& path) const {
        return const_cast<index_entry*>(this)->entry(path);
    }

    index_flags index_entry::get(const path& path) const {
        const index_entry* e = entry(path);
        return e != nullptr ? e->flags : index_flags();
    }
    
    void index_entry::set(const path& path, index_flags flags) {
        if (path.empty()) {
            this->flags = flags;
            return;
        }

        const auto removal_pair = compute_removal_pair(path, flags);
        if (removal_pair.first == nullptr) {
            create_entry(path).flags = flags;
            return;
        }
            
        if (removal_pair.second != nullptr) {
            removal_pair.first->entries.erase(*removal_pair.second);
        }
    }

    std::pair<index_entry*, const std::string*> index_entry::compute_removal_pair(const std::filesystem::path& path, index_flags flags) {
        
        index_entry* e = this;
        std::pair<index_entry*, const std::string*> removal_pair(flags.is_default() ? e : nullptr, nullptr);
        if (removal_pair.first == nullptr) {
            return removal_pair;
        }

        bool in_recursive = false;
        bool in_shallow = false;

        for (const auto& el: path) {
            in_recursive |= e->flags.mode == index_sync_mode::RECURSIVE;
            in_shallow = e->flags.mode == index_sync_mode::SHALLOW;

            std::string key = el.string();
            auto eit = e->entries.find(key);
            if (eit == e->entries.end()) {
                break;
            }

            if (removal_pair.second == nullptr || e->entries.size() > 1 || !e->flags.is_default()) {
                removal_pair.first = e;
                removal_pair.second = &eit->first;
            }

            e = &eit->second;
        }
        
        if (in_recursive || in_shallow) {
            removal_pair.first = nullptr;
        }

        return removal_pair;
    }

    index_entry& index_entry::create_entry(const std::filesystem::path& path) {
        index_entry* e = this;
        for (const auto& el: path) {
            e = &e->entries[el.string()];
        }
        return *e;
    }

}