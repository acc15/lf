#pragma once

#include <vector>
#include <filesystem>
#include <source_location>

namespace lf {

    extern const std::filesystem::path test_dir_path;
    extern const std::filesystem::path test_root_path;
    std::string test_root_str(std::string_view p); 

}