#pragma once

#include "tree/tree.hpp"

namespace lf {

    template <tree_type Tree>
    class tracked_tree {
    public:

        using tree_type = Tree;
        using data_type = typename Tree::data_type;
        using node_type = typename Tree::node_type;
        using path = std::filesystem::path;

        tree_type root;
        bool changed = false;

        tracked_tree(): root(), changed(false) {}
        tracked_tree(const tree_type& init): root(init), changed(false) {}

        const node_type* node(const std::filesystem::path& path) const {
            return root.node(path);
        }

        data_type get(const std::filesystem::path& path) const {
            return root.get(path);
        }

        void remove(const std::filesystem::path& path, bool empty_only = false) {
            changed |= root.remove(path, empty_only);
        }

        void set(const std::filesystem::path& path, const data_type& value, bool remove_children = false) {
            changed |= root.set(path, value, remove_children);
        }

        void move(const std::filesystem::path& src, const std::filesystem::path& dst) {
            changed |= root.move(src, dst);
        }

    };

}