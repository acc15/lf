#include "sync/path_info.hpp"

namespace lf {

    path_info::path_info(bool local, const std::filesystem::path& path):
        local(local),
        name(local ? "local" : "remote"),
        path(path), 
        status(std::filesystem::status(path)), 
        type(status.type())  
    {
    }


}