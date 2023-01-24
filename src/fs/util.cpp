#include <algorithm>

#include "fs/util.hpp"

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

    void create_parent_dirs(const fs::path& path) {
        if (path.has_parent_path()) {
            fs::create_directories(path.parent_path());
        }
    }

    void copy_file_with_timestamp(const fs::path& src, const fs::path& dst) {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        fs::file_time_type src_time = fs::last_write_time(src);
        fs::last_write_time(dst, src_time);
    }

}