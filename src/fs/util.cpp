#include <algorithm>

#include "fs/util.hpp"
#include "log/log.hpp"

namespace fs = std::filesystem;

namespace lf {

    fs::path normalize_path(const fs::path& p) {
        fs::path path = p.empty() ? fs::current_path() : fs::absolute(p).lexically_normal();
        return path.has_parent_path() && path.filename().empty() ? path.parent_path() : path;
    }

    bool is_subpath(const fs::path& path, const fs::path& base) {
        const auto mismatch_pair = std::mismatch(path.begin(), path.end(), base.begin(), base.end());
        return mismatch_pair.second == base.end();
    }

    fs::path relative_path(const fs::path& path, const fs::path& base) {
        fs::path rel_path = path.lexically_relative(base);
        if (rel_path == ".") {
            rel_path.clear();
        }
        return rel_path;
    }

    fs::path join_path(const fs::path& path, const fs::path& rel) {
        return rel.empty() ? path : path / rel;
    }

    bool create_parent_dirs(const fs::path& path) {
        return path.has_parent_path() ? fs::create_directories(path.parent_path()) : false;
    }

    void copy_file_with_timestamp(const fs::path& src, const fs::path& dst) {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        fs::file_time_type src_time = fs::last_write_time(src);
        fs::last_write_time(dst, src_time);
    }

    std::filesystem::filesystem_error make_fs_error(const std::string& what, const std::filesystem::path& path) {
        std::error_code ec(errno, std::iostream_category());
        return std::filesystem::filesystem_error(what, path, ec);
    }

    void throw_fs_error(const std::string& what, const std::filesystem::path& path) {
        throw make_fs_error(what, path);
    }

    void throw_fs_error_if_exists(const std::string& what, const std::filesystem::path& path) {
        std::filesystem::filesystem_error err = make_fs_error(what, path);
        if (err.code().value() != ENOENT) {
            throw err;
        }
        log.debug() && log() << err.what() << log::end;
    }

}