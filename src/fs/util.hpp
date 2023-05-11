#pragma once

#include <filesystem>
#include <optional>

namespace lf {

    using path_pair = std::pair<std::filesystem::path, std::filesystem::path>;

    bool is_subpath(const std::filesystem::path& path, const std::filesystem::path& base);
    std::filesystem::path subpath(
        std::filesystem::path::const_iterator begin,
        std::filesystem::path::const_iterator end
    );
    
    std::filesystem::path absolute_path(const std::filesystem::path& p);
    std::optional<std::filesystem::path> relative_path(
        const std::filesystem::path& path, 
        const std::filesystem::path& base
    );

    std::optional<path_pair> normalize_rel(
        const std::filesystem::path& path, 
        const std::filesystem::path& base
    );

    std::optional<path_pair> check_move(
        const std::filesystem::path& from, 
        const std::filesystem::path& to,
        bool fs_checks = true
    );

    bool create_parent_dirs(const std::filesystem::path& path);
    std::filesystem::path join_path(const std::filesystem::path& path, const std::filesystem::path& rel);
    void copy_file_with_timestamp(const std::filesystem::path& src, const std::filesystem::path& dst);

    void move_path(const std::filesystem::path& src, const std::filesystem::path& dst);

    void throw_fs_error(const std::string& what, const std::filesystem::path& path);
    void throw_fs_error_if_exists(const std::string& what, const std::filesystem::path& path);
    
}