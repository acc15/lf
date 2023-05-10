#pragma once

#include <string>
#include <filesystem>
#include <map>
#include <concepts>

namespace lf {

    template<typename T>
    concept tree_data_type = std::default_initializable<T> && std::equality_comparable<T>;

    template <tree_data_type T>
    struct tree {
        
        using data_type = T;
        using map_type = std::map<std::string, tree>;
        using entry_ptr = typename map_type::const_pointer;
        using map_iter = typename map_type::iterator;
        using removal_pair = std::pair<map_type*, map_iter>;
        using node_type = tree;

        data_type data = {};
        map_type entries = {};

        tree* node(const std::filesystem::path& path = std::filesystem::path()) {
            tree* e = this;
            for (const auto& el: path) {
                const auto it = e->entries.find(el.string());
                if (it == e->entries.end()) {
                    return nullptr;
                }
                e = &it->second;
            }
            return e;
        }

        const tree* node(const std::filesystem::path& path = std::filesystem::path()) const {
            return const_cast<tree*>(this)->node(path);
        }

        data_type get(const std::filesystem::path& path) const {
            const tree* e = node(path);
            return e != nullptr ? e->data : data_type {};
        }

        bool move(const std::filesystem::path& src, const std::filesystem::path& dst) {
            namespace fs = std::filesystem;

            removal_pair removal;
            node_type* node = this;
            for (const fs::path& el: src) {
                const auto it = node->entries.find(el.string());
                if (it == node->entries.end()) {
                    return false;
                }
                removal = std::make_pair(&node->entries, it);
                node = &it->second;
            }
            return false;
        }

        bool set(const std::filesystem::path& path, const T& new_data, bool remove_children = false) {
            bool modified = false;

            tree* n = this;
            for (const std::filesystem::path& el: path) {
                const auto r = n->entries.emplace(std::move(el.string()), tree {});
                if (r.second) {
                    modified = true;
                }
                n = &r.first->second;
            }
            if (new_data != n->data) {
                n->data = new_data;
                modified = true;
            }
            if (remove_children && !n->entries.empty()) {
                entries.clear();
                modified = true;
            }
            return modified;
        }

        bool remove(const std::filesystem::path& path, bool empty_only = false) {

            tree* n = this;

            removal_pair removal = std::make_pair(nullptr, map_iter());
            for (const auto& el: path) {
                removal.first = &n->entries;
                removal.second = n->entries.find(el.string());
                if (removal.second == n->entries.end()) {
                    return false;
                }
                n = &removal.second->second;
            }

            if (empty_only && !n->entries.empty()) {
                if (n->data == data_type{}) {
                    return false;
                }
                n->data = data_type{};
                return true;
            }

            removal.first->erase(removal.second);
            return true;
        }

    };

    template <typename Tree>
    concept tree_type = std::derived_from<Tree, tree<typename Tree::data_type>>;

}

