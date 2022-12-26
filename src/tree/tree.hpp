#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <concepts>

namespace lf {

    template <std::default_initializable T> requires std::equality_comparable<T>
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
            return const_cast<node_type*>(this)->node(path);
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
            value_type default_value{};

            std::pair<node_type*, const std::string*> removal_pair(nullptr, nullptr);
            if (data != default_value) {
                return removal_pair;
            }

            node_type* e = this;
            removal_pair.first = e;

            for (const auto& el: path) {
                std::string key = el.string();
                auto eit = e->entries.find(key);
                if (eit == e->entries.end()) {
                    removal_pair.second = nullptr;
                    return removal_pair;
                }
                if (removal_pair.second == nullptr || e->entries.size() > 1 || e->data != default_value) {
                    removal_pair.first = e;
                    removal_pair.second = &eit->first;
                }
                e = &eit->second;
            }

            if (!e->entries.empty()) {
                removal_pair.second = nullptr;
            }

            return removal_pair;
        }

    };

}

