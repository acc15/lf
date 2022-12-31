#include "fs/error.hpp"

namespace lf {

    file_not_found_error::file_not_found_error(const std::filesystem::path& path, const std::string& what) : 
        std::filesystem::filesystem_error(what, path, std::make_error_code(std::errc::no_such_file_or_directory))
    {
    }

    void throw_fs_error(const std::filesystem::path& path, const std::string& what) {
        std::error_code code(errno, std::generic_category());
        if (code.value() == ENOENT) {
            throw file_not_found_error(path, what);
        }
        throw std::filesystem::filesystem_error(what, path, code);
    }

}