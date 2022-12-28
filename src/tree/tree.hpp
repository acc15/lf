#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <concepts>

namespace lf {

    template<typename T>
    concept tree_data = std::default_initializable<T> && std::equality_comparable<T>;

    template <tree_data T>
    struct tree {
        
        using value_type = T;
        using entry_map = std::unordered_map<std::string, tree<T>>;
        using tree_type = tree<value_type>;

        static const T default_data;

        T data = {};
        entry_map entries = {};

        tree_type* node(const std::filesystem::path& path) {
            tree_type* e = this;
            for (const auto& el: path) {
                const auto it = e->entries.find(el.string());
                if (it == e->entries.end()) {
                    return nullptr;
                }
                e = &it->second;
            }
            return e;
        }

        const tree_type* node(const std::filesystem::path& path) const {
            return const_cast<tree_type*>(this)->node(path);
        }

        bool set(const value_type& other) {
            if (data == other) {
                return false;
            }
            data = other;
            return true;
        }

        value_type get(const std::filesystem::path& path) const {
            const tree_type* e = node(path);
            return e != nullptr ? e->data : value_type();
        }

        bool set(const std::filesystem::path& path, const value_type& data) {
            return path.empty() ? set(data) : data != default_data 
                ? create_node(path).set(data) 
                : set_default(path);
        }

    private:
        tree_type& create_node(const std::filesystem::path& path) {
            tree_type* e = this;
            for (const auto& el: path) {
                e = &e->entries[el.string()];
            }
            return *e;
        }

        bool set_default(const std::filesystem::path& path) {

            tree_type* e = this;

            tree_type* removal_node = e;
            const std::string* removal_key = nullptr;

            for (const auto& el: path) {
                std::string key = el.string();
                auto eit = e->entries.find(key);
                if (eit == e->entries.end()) {
                    return removal_node->remove(e->entries.empty() ? removal_key : nullptr);
                }
                if (removal_key == nullptr || e->entries.size() > 1 || e->data != default_data) {
                    removal_node = e;
                    removal_key =  &eit->first;
                }
                e = &eit->second;
            }

            // e pointing to node specified by path
            return e->entries.empty() 
                ? removal_node->remove(removal_key) 
                : e->set(default_data);
        }

        bool remove(const std::string* key_ptr) {
            if (key_ptr == nullptr) {
                return false;
            }
            entries.erase(*key_ptr);
            return true;
        }

    };

    template <tree_data T> const T tree<T>::default_data = {};

}

