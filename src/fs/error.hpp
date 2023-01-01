#pragma once

#include <filesystem>
#include <exception>
#include <string>

namespace lf {

    struct file_not_found_error: std::filesystem::filesystem_error {
        file_not_found_error(const std::string& what, const std::filesystem::path& path, const std::error_code& code);
    };

    void throw_fs_error(const std::string& what, const std::filesystem::path& path);

}