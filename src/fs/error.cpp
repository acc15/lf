#include "fs/error.hpp"
#include "log/log.hpp"

namespace lf {

    std::error_code make_io_error_code() {
        return std::error_code(errno, std::iostream_category());
    }

    file_not_found_error::file_not_found_error(const std::string& what, const std::filesystem::path& path, const std::error_code& code) : 
        std::filesystem::filesystem_error(what, path, code)
    {
    }

    void throw_fs_error(const std::string& what, const std::filesystem::path& path, bool optional) {
        std::error_code code = make_io_error_code();
        if (code.value() != ENOENT) {
            throw std::filesystem::filesystem_error(what, path, code);
        }

        file_not_found_error fnf(what, path, code);
        if (!optional) {
            throw fnf;
        }

        log.debug() && log() << fnf.what() << log::end;
    }

}