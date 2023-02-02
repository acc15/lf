#pragma once

#include <filesystem>

namespace lf {

    
    enum class os_path_kind {
        LOG,
        CONFIG
    };

    std::filesystem::path get_os_base_path(os_path_kind kind);

}