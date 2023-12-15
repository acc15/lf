#include "glob/glob_path.hpp"

#include <ranges>

namespace lf {

glob_path::glob_path(const std::initializer_list<element>& elements_init): elements(elements_init) {}

bool glob_path::matches(const std::filesystem::path& path) {
    const auto path_str_range = path | std::views::transform([](const std::filesystem::path& p) { 
        return p.string();
    }); 
    return matches(path_str_range);
}

}

