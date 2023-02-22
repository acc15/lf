#pragma once

#include "log/log.hpp"
#include "fs/serialization.hpp"
#include "tree/binary.hpp"

namespace lf {

    template <serializable_tree_type Tree>
    class tracked_tree {
    public:

        using tree_type = Tree;
        using data_type = typename Tree::data_type;
        using node_type = typename Tree::node_type;
        using path = std::filesystem::path;

        tree_type root;
        bool changed = false;

        bool empty() const {
            return root.empty();
        }

        const node_type* node() const {
            return &root;
        }

        const node_type* node(const std::filesystem::path& path) const {
            return root.node(path);
        }

        data_type get() const {
            return root.data;
        }

        data_type get(const std::filesystem::path& path) const {
            return root.get(path);
        }

        void remove(const std::filesystem::path& path, bool empty_only = false) {
            changed |= root.remove(path, empty_only);
        }

        void set(const data_type& value, bool remove_children = false) {
            changed |= root.set(value, remove_children);
        }

        void set(const std::filesystem::path& path, const data_type& value, bool remove_children = false) {
            changed |= root.set(path, value, remove_children);
        }

    };

}