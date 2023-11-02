#include "sync/path_info.hpp"

#include "fs/util.hpp"
#include "fs/time.hpp"

namespace lf {

    path_info::path_info(bool is_left, const std::filesystem::path& root_path, const std::filesystem::path& item_path):
        is_left(is_left),
        name(is_left ? "left" : "right"),
        root(root_path),
        item(item_path),
        full_path(join_path(root_path, item_path)),
        status(std::filesystem::status(full_path)), 
        type(status.type()),
        time{}
    {
    }

    path_info path_info::parent() const {
        return path_info(is_left, root, item.parent_path());
    }

    void path_info::init_time() {
        time = ntfs_last_write_time(full_path);
    }

}