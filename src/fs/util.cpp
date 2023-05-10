#include <algorithm>
#include <optional>

#include "fs/util.hpp"
#include "log/log.hpp"

namespace fs = std::filesystem;

namespace lf {

    bool is_subpath(const fs::path& path, const fs::path& base) {
        const auto mismatch_pair = std::mismatch(path.begin(), path.end(), base.begin(), base.end());
        return mismatch_pair.second == base.end();
    }

    fs::path absolute_path(const fs::path& p) {
        fs::path path = p.empty() ? fs::current_path() : fs::absolute(p).lexically_normal();
        return path.has_parent_path() && path.filename().empty() ? path.parent_path() : path;
    }

    fs::path subpath(fs::path::const_iterator begin, fs::path::const_iterator end) {
        fs::path result;
        for (;begin != end; ++begin) {
            result /= *begin;
        }
        return result;
    }

    std::optional<fs::path> relative_path(const fs::path& path, const fs::path& base) {
        auto pi = path.begin();
        for (auto bi = base.begin(); bi != base.end(); bi++, pi++) {
            if (pi == path.end() || *pi != *bi) {
                return std::nullopt;
            }
        }
        return subpath(pi, path.end());
    }

    std::optional<rel_path_info> make_rel_path_info(const fs::path& path, const fs::path& base) {
        fs::path abs;
        try {
            abs = absolute_path(path);
        } catch (const fs::filesystem_error& ex) {
            log.error() && log() 
                << "unable to make absolute path from " << path 
                << ", error: " << ex.what() 
                << log::end;
            return std::nullopt;
        }
        const auto rel = relative_path(abs, base);
        if (!rel) {
            log.error() && log()
                << "path " << abs 
                << " is outside of base directory " << base 
                << log::end;
            return std::nullopt;
        }
        return rel_path_info { abs, *rel };
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

    fs::filesystem_error make_fs_error(const std::string& what, const fs::path& path) {
        std::error_code ec(errno, std::iostream_category());
        return fs::filesystem_error(what, path, ec);
    }

    void throw_fs_error(const std::string& what, const fs::path& path) {
        throw make_fs_error(what, path);
    }

    void throw_fs_error_if_exists(const std::string& what, const fs::path& path) {
        fs::filesystem_error err = make_fs_error(what, path);
        if (err.code().value() != ENOENT) {
            throw err;
        }
        log.debug() && log() << err.what() << log::end;
    }

}