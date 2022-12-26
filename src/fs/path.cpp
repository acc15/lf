#include <algorithm>

#include "path.hpp"

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

}