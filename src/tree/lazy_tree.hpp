#pragma once

#include <optional>
#include <filesystem>

#include "io/adv_fstream.hpp"
#include "tree/tracked_tree.hpp"
#include "tree/binary.hpp"
#include "fs/serialization.hpp"

namespace lf {

    template <serializable_tree_type Tree>
    class lazy_tree {
    public:
        using tree_type = tracked_tree<Tree>;

    private:
        std::filesystem::path path;
        std::optional<tree_type> value;
        adv_fstream file;

        void load_tree(Tree& tree) {
            create_parent_dirs(path);
            open_and_lock<tree_binary_format, Tree>(path, file, OPEN_READ_WRITE_LOCK);
            if (file.tellg() == 0) {
                // new empty file, skip loading
                return;
            }
            
            file.seekg(0);
            load_file<tree_binary_format>(path, file, tree);
        }

    public:
        lazy_tree(const std::filesystem::path& path): path(path) {}
        lazy_tree(const std::filesystem::path& path, const Tree& init): path(path), value(init) {}
        ~lazy_tree() { save_if_changed(); }
 
        tree_type& operator*() {
            if (!value) {
                value = tree_type {};
                load_tree(value->root);
            }
            return *value;
        }

        tree_type* operator->() {
            return &operator*();
        }

        bool save_and_close() {
            bool saved = save_if_changed();
            file.close();
            return saved;
        }

        bool save_if_changed() {
            if (!file.is_open() || !value.has_value() || !value->changed) {
                return false;
            }
            file.truncate();
            save_file<tree_binary_format, Tree>(path, file, value->root);

            value->changed = false;
            return true;
        }

    };

}