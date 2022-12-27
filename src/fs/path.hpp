#include <filesystem>

namespace lf {

    std::filesystem::path normalize_path(const std::filesystem::path& p);
    bool is_subpath(const std::filesystem::path& path, const std::filesystem::path& base);
    std::filesystem::path relative_path(const std::filesystem::path& path, const std::filesystem::path& base);
    bool create_parent_dirs(const std::filesystem::path& path);

}