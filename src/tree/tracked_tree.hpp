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

    private:
        bool _mod = false;
        tree_type _root;

    public:
        tracked_tree& operator=(const tree_type& v) {
            _root = v;
            return *this;
        }

        bool empty() const {
            return _root.empty();
        }

        bool is_changed() const {
            return _mod;
        }

        void load(const path& path, bool optional = false) {
            _mod = false;
            load_file(path, _root, optional);
        }

        void save_if_changed(const path& path) {
            if (_mod) {
                save_file(path, _root);
                _mod = false;
            } else {
                log.debug() && log() << "skipped save of " << tree_type::name << " to " << path << " as it wasn't modified" << log::end;
            }
        }

        tree_type& root() {
            return _root;
        }

        const tree_type& root() const {
            return _root;
        }

        const node_type* node() const {
            return &_root;
        }

        const node_type* node(const std::filesystem::path& path) const {
            return _root.node(path);
        }

        data_type get() const {
            return _root.data;
        }

        data_type get(const std::filesystem::path& path) const {
            return _root.get(path);
        }

        void remove(const std::filesystem::path& path, bool empty_only = false) {
            _mod |= _root.remove(path, empty_only);
        }

        void set(const data_type& value, bool remove_children = false) {
            _mod |= _root.set(value, remove_children);
        }

        void set(const std::filesystem::path& path, const data_type& value, bool remove_children = false) {
            _mod |= _root.set(path, value, remove_children);
        }

    };

}