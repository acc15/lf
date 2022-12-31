#pragma once

#include <filesystem>
#include <exception>
#include <string>

namespace lf {

    struct file_not_found_error: std::filesystem::filesystem_error {
        file_not_found_error(const std::filesystem::path& path, const std::string& what);
    };

    void throw_fs_error(const std::filesystem::path& path, const std::string& what);

}