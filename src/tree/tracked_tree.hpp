#pragma once

#include "io/log.hpp"
#include "fs/serialization.hpp"
#include "tree/binary.hpp"

namespace lf {

    template <serializable_tree_concept Tree>
    class tracked_tree {
    public:
        using tree_type = Tree;
        using data_type = typename Tree::data_type;
        using node_type = typename Tree::node_type;
        using path = std::filesystem::path;

    private:
        bool changed = false;
        tree_type root;

    public:
        tracked_tree& operator=(const tree_type& v) {
            root = v;
            return *this;
        }

        bool is_changed() const {
            return changed;
        }

        void load(const path& path) {
            changed = false;
            load_file(path, root);
        }

        void save_if_changed(const path& path) {
            if (changed) {
                save_file(path, root);
                changed = false;
            } else {
                log.debug() && log() << "skipped save of " << tree_type::name << " to " << path << " as it wasn't modified" << std::endl;
            }
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

        void remove(const std::filesystem::path& path) {
            changed |= root.remove(path);
        }

        void set(const data_type& value) {
            changed |= root.set(value);
        }

        void set(const std::filesystem::path& path, const data_type& value) {
            changed |= root.set(path, value);
        }

    };

}