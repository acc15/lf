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
            if (path.empty()) {
                return set(new_data, remove_children);
            }

            bool modified = false;
            tree* n = this;
            for (const std::filesystem::path& el: path) {
                const std::pair<typename map_type::iterator, bool> r = n->entries.emplace(std::move(el.string()), tree {});
                if (r.second) {
                    modified = true;
                }
                n = &r.first->second;
            }
            modified |= n->set(new_data, remove_children);
            return modified;
        }

        bool remove(const std::filesystem::path& path, bool empty_only = false) {
            if (path.empty()) {
                return set(data_type {}, !empty_only);
            }

            tree* node = this;
            tree* removal_node = nullptr;
            typename map_type::iterator remove_iter;
            for (const std::filesystem::path& el: path) {
                remove_iter = node->entries.find(el.string());
                if (remove_iter == node->entries.end()) {
                    return false;
                }
                removal_node = node;
                node = &remove_iter->second;
            }

            if (empty_only && !node->entries.empty()) {
                return node->set(data_type {}, false);
            }

            removal_node->entries.erase(remove_iter);
            return true;
        }

    };

    template <typename Tree>
    concept tree_concept = std::derived_from<Tree, tree<typename Tree::data_type>>;

}

