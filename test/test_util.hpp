#pragma once

#include <vector>
#include <filesystem>
#include <source_location>

namespace lf {

    extern const std::filesystem::path test_dir;
    extern const std::filesystem::path test_root;

    std::filesystem::path test_path(std::string_view p); 
    std::string test_pstr(std::string_view p); 

}