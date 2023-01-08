#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <concepts>

namespace lf {

    template<typename T>
    concept tree_data = std::default_initializable<T> && std::equality_comparable<T>;

    template <tree_data T, template<typename...> typename Map = std::unordered_map>
    struct tree {
        
        using data_type = T;
        using map_type = Map<std::string, tree>;
        using entry_ptr = typename map_type::const_pointer;

        data_type data = {};
        map_type entries = {};

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

        bool set(const data_type& other) {
            if (data == other) {
                return false;
            }
            data = other;
            return true;
        }

        data_type get(const std::filesystem::path& path) const {
            const tree* e = node(path);
            return e != nullptr ? e->data : T();
        }

        bool set(const std::filesystem::path& path, const T& new_data) {
            return path.empty() ? set(new_data) : new_data != data_type {}
                ? create_node(path).set(new_data)
                : set_default(path, true);
        }

        bool remove(const std::filesystem::path& path) {
            return set_default(path, false);
        }

    private:
        tree& create_node(const std::filesystem::path& path) {
            tree* e = this;
            for (const auto& el: path) {
                e = &e->entries[el.string()];
            }
            return *e;
        }

        bool set_default(const std::filesystem::path& path, bool keep_default_subtree) {

            tree* e = this;

            tree* removal_node = e;
            const std::string* removal_key = nullptr;

            for (const auto& el: path) {
                std::string key = el.string();
                auto eit = e->entries.find(key);
                if (eit == e->entries.end()) {
                    return e->entries.empty() ? removal_node->remove(removal_key) : false;
                }
                if (removal_key == nullptr || e->entries.size() > 1 || e->data != data_type {}) {
                    removal_node = e;
                    removal_key =  &eit->first;
                }
                e = &eit->second;
            }

            // e pointing to node specified by path
            return !keep_default_subtree || e->entries.empty() 
                ? removal_node->remove(removal_key) 
                : e->set({});
        }

        bool remove(const std::string* key_ptr) {
            if (key_ptr == nullptr) {
                return false;
            }
            entries.erase(*key_ptr);
            return true;
        }

    };

    template <typename Tree>
    concept tree_concept = requires (Tree& t) {
        // using IIFE to describe tree with template-template parameter
        []<tree_data T, template <typename...> typename Map>(tree<T, Map>&){}(t);
    };

}

