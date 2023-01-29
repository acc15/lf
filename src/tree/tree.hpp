#pragma once

#include <string>
#include <filesystem>
#include <map>
#include <concepts>

namespace lf {

    template<typename T>
    concept tree_data_concept = std::default_initializable<T> && std::equality_comparable<T>;

    template <tree_data_concept T>
    struct tree {
        
        using data_type = T;
        using map_type = std::map<std::string, tree>;
        using entry_ptr = typename map_type::const_pointer;
        using node_type = tree;

        data_type data = {};
        map_type entries = {};

        bool empty() const {
            return entries.empty();
        }

        tree* node(const std::filesystem::path& path) {
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

        const tree* node(const std::filesystem::path& path) const {
            return const_cast<tree*>(this)->node(path);
        }

        data_type get() const {
            return data;
        }

        data_type get(const std::filesystem::path& path) const {
            const tree* e = node(path);
            return e != nullptr ? e->data : data_type {};
        }

        bool set(const data_type& other, bool remove_children = false) {
            bool modified = false;
            if (data != other) {
                data = other;
                modified = true;
            }
            if (remove_children && !entries.empty()) {
                entries.clear();
                modified = true;
            }
            return modified;
        }

        bool set(const std::filesystem::path& path, const T& new_data, bool remove_children = false) {
            return path.empty() ? set(new_data, remove_children) : new_data != data_type {}
                ? create_node(path).set(new_data, remove_children)
                : set_default(path, remove_children);
        }

        bool remove(const std::filesystem::path& path) {
            return set_default(path, true);
        }

    private:
        tree& create_node(const std::filesystem::path& path) {
            tree* e = this;
            for (const auto& el: path) {
                e = &e->entries[el.string()];
            }
            return *e;
        }

        bool set_default(const std::filesystem::path& path, bool remove_children) {

            tree* e = this;

            tree* removal_node = e;
            const std::string* removal_key = nullptr;

            for (const auto& el: path) {
                std::string key = el.string();
                auto eit = e->entries.find(key);
                if (eit == e->entries.end()) {
                    return e->entries.empty() && e->data == data_type {} ? removal_node->erase(removal_key) : false;
                }
                if (removal_key == nullptr || e->entries.size() > 1 || e->data != data_type {}) {
                    removal_node = e;
                    removal_key =  &eit->first;
                }
                e = &eit->second;
            }

            // e pointing to node specified by path
            return remove_children || e->entries.empty() 
                ? removal_node->erase(removal_key) 
                : e->set({});
        }

        bool erase(const std::string* key_ptr) {
            return key_ptr != nullptr && entries.erase(*key_ptr) != 0;
        }

    };

    template <typename Tree>
    concept tree_concept = std::derived_from<Tree, tree<typename Tree::data_type>>;

}

