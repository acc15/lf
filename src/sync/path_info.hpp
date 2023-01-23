#pragma once

#include <string_view>
#include <filesystem>

namespace lf {

    struct path_info {
        std::string_view name;
        std::filesystem::path path;
        std::filesystem::file_status status;
        std::filesystem::file_type type;
        path_info(std::string_view name, const std::filesystem::path& sync_path, const std::filesystem::path& rel_path);
    };

}