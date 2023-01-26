#pragma once

#include <filesystem>

namespace lf {

    struct path_info {
        bool local;
        const char* name;
        std::filesystem::path path;
        std::filesystem::file_status status;
        std::filesystem::file_type type;

        path_info(bool local, const std::filesystem::path& path);

    };

}