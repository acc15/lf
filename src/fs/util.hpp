#pragma once

#include <filesystem>
#include <optional>

namespace lf {

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

    struct rel_path_info {
        std::filesystem::path abs;
        std::filesystem::path rel;
    };

    std::optional<rel_path_info> make_rel_path_info(
        const std::filesystem::path& path, 
        const std::filesystem::path& base
    );

    bool create_parent_dirs(const std::filesystem::path& path);
    std::filesystem::path join_path(const std::filesystem::path& path, const std::filesystem::path& rel);
    void copy_file_with_timestamp(const std::filesystem::path& src, const std::filesystem::path& dst);

    void throw_fs_error(const std::string& what, const std::filesystem::path& path);
    void throw_fs_error_if_exists(const std::string& what, const std::filesystem::path& path);
    
}