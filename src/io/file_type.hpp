#pragma once

#include <filesystem>
#include <unordered_map>

namespace lf {

    extern const std::unordered_map<std::filesystem::file_type, const char*> file_type_names;

    std::ostream& operator<<(std::ostream& s, const std::filesystem::file_type& t);

}