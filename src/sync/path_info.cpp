#include "sync/path_info.hpp"

#include "fs/util.hpp"
#include "fs/time.hpp"

namespace lf {

    path_info::path_info(bool local, const std::filesystem::path& root_path, const std::filesystem::path& item_path):
        local(local),
        name(local ? "local" : "remote"),
        root(root_path),
        item(item_path),
        path(join_path(root_path, item_path)),
        status(std::filesystem::status(path)), 
        type(status.type()),
        time{}
    {
    }

    path_info path_info::parent() const {
        return path_info(local, root, item.parent_path());
    }

    void path_info::init_time() {
        time = ntfs_last_write_time(path);
    }

}