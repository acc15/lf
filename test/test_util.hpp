#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace lf {

    extern const std::filesystem::path test_dir_path;
    extern const std::filesystem::path test_root_path;
    std::string test_root_str(std::string_view p); 
    std::filesystem::path create_temp_test_dir(const std::filesystem::path& suffix = std::filesystem::path());
    void write_text(const std::filesystem::path& path, const std::string& text);
    std::string read_text(const std::filesystem::path& path);

}