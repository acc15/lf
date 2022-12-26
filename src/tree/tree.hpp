#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>

namespace lf {

    template <typename T>
    struct tree {
        
        using entry_map = std::unordered_map<std::string, tree<T>>;
        using value_type = T;
        using node_type = tree<value_type>;

        T data;
        entry_map entries = {};

        node_type* node(const std::filesystem::path& path) {
            node_type* e = this;
            for (const auto& el: path) {
                const auto it = e->entries.find(el.string());
                if (it == e->entries.end()) {
                    return nullptr;
                }
                e = &it->second;
            }
            return e;
        }

        const node_type* node(const std::filesystem::path& path) const {
            return const_cast<node_type*>(this)->entry(path);
        }

        value_type get(const std::filesystem::path& path) const {
            const node_type* e = node(path);
            return e != nullptr ? e->data : value_type();
        }

        void set(const std::filesystem::path& path, const value_type& data) {
            if (path.empty()) {
                this->data = data;
                return;
            }
            const auto removal_pair = compute_removal_pair(path, data);
            if (removal_pair.first == nullptr) {
                create_entry(path).data = data;
                return;
            }
            if (removal_pair.second != nullptr) {
                removal_pair.first->entries.erase(*removal_pair.second);
            }
        }

    private:
        node_type& create_entry(const std::filesystem::path& path) {
            node_type* e = this;
            for (const auto& el: path) {
                e = &e->entries[el.string()];
            }
            return *e;
        }

        std::pair<node_type*, const std::string*> compute_removal_pair(const std::filesystem::path& path, const value_type& data) {
            node_type* e = this;
            std::pair<node_type*, const std::string*> removal_pair(data == value_type() ? e : nullptr, nullptr);
            if (removal_pair.first == nullptr) {
                return removal_pair;
            }

            for (const auto& el: path) {
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
            return removal_pair;
        }

    };

}

