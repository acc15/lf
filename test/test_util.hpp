#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace lf {

    extern const std::filesystem::path test_dir_path;
    extern const std::filesystem::path test_root_path;
    std::string test_root_str(std::string_view p); 

}