#pragma once

#include <filesystem>

namespace lf {

    struct path_info {
        bool local;
        const char* name;
        std::filesystem::path root;
        std::filesystem::path item;
        std::filesystem::path path;
        std::filesystem::file_status status;
        std::filesystem::file_type type;
        std::filesystem::file_time_type time;

        path_info(bool local, const std::filesystem::path& root_path, const std::filesystem::path& item_path);

        path_info parent() const;

        void init_time();

    };

}