#include "sync/path_info.hpp"

namespace lf {

    path_info::path_info(std::string_view name, const std::filesystem::path& sync_path, const std::filesystem::path& rel_path):
        name(name),
        path(rel_path.empty() ? sync_path : sync_path / rel_path), 
        status(std::filesystem::status(path)), 
        type(status.type())  
    {
    }

}