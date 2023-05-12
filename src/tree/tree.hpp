#pragma once

#include <string>
#include <filesystem>
#include <map>
#include <concepts>
#include <optional>

namespace lf {

    template<typename T>
    concept tree_data_type = std::default_initializable<T> && std::equality_comparable<T>;

    template <tree_data_type T>
    class tree {
    public:
        using data_type = T;
        using map_type = std::map<std::string, tree>;
        using entry_ptr = typename map_type::const_pointer;
        using map_iter = typename map_type::iterator;
        using rm_pair = std::pair<map_type*, map_iter>;
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
            if (src.empty()) {
                return false;
            }
            
            auto rm = find_rm_pair(src);
            if (!rm) {
                return false;
            }

            bool modified = false;
            auto& src_node = node_from_rm_pair(*rm);
            auto& dst_node = create_node(dst, modified);
            dst_node = std::move(src_node);
            modified |= erase_node(*rm);
            return modified;
        }

        bool set(const std::filesystem::path& path, const T& new_data, bool remove_children = false) {
            bool modified = false;
            auto& n = create_node(path, modified);
            modified |= n.set_if_different(new_data);
            modified |= (remove_children && n.clear_not_empty());
            return modified;
        }

        bool remove(const std::filesystem::path& path, bool empty_only = false) {
            auto rm = find_rm_pair(path);
            if (!rm) {
                return false;
            }

            auto& n = node_from_rm_pair(*rm);
            if (rm->first == nullptr || (empty_only && !n.entries.empty())) {
                return n.set_if_different(data_type{});
            }
            return erase_node(*rm);
        }

    private:
        node_type& create_node(const std::filesystem::path& path, bool& modified) {
            node_type* n = this;
            for (const std::filesystem::path& el: path) {
                const auto r = n->entries.emplace(std::move(el.string()), node_type {});
                if (r.second) {
                    modified = true;
                }
                n = &r.first->second;
            }
            return *n;
        }

        std::optional<rm_pair> find_rm_pair(const std::filesystem::path& path) {
            rm_pair rm;
            for (const auto& el: path) {
                auto& n = node_from_rm_pair(rm).entries;
                rm = std::make_pair(&n, n.find(el.string()));
                if (rm.second == rm.first->end()) {
                    return std::nullopt;
                }
            }
            return rm;
        }

        node_type& node_from_rm_pair(const rm_pair& pair) {
            return pair.first == nullptr ? *this : pair.second->second;
        }

        bool erase_node(rm_pair& pair) {
            if (pair.first == nullptr) {
                return false;
            }
            pair.first->erase(pair.second);
            return true;
        }

        bool clear_not_empty() {
            if (entries.empty()) {
                return false;
            }
            entries.clear();
            return true;
        }

        bool set_if_different(const data_type& val) {
            if (data == val) {
                return false;
            }
            data = val;
            return true;
        }

    };

    template <typename Tree>
    concept tree_type = std::derived_from<Tree, tree<typename Tree::data_type>>;

    template <typename Tree>
    concept serializable_tree_type = tree_type<Tree> && requires {
        { Tree::file_signature } -> std::convertible_to<const char*>;
        { Tree::file_version } -> std::convertible_to<std::uint8_t>;
    };

}

