#include "fs/error.hpp"

namespace lf {

    file_not_found_error::file_not_found_error(const std::string& what, const std::filesystem::path& path, const std::error_code& code) : 
        std::filesystem::filesystem_error(what, path, code)
    {
    }

    void throw_fs_error(const std::string& what, const std::filesystem::path& path) {
        std::error_code code(errno, std::generic_category());
        if (code.value() == ENOENT) {
            throw file_not_found_error(what, path, code);
        }
        throw std::filesystem::filesystem_error(what, path, code);
    }

}