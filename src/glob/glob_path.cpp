#include "glob/glob_path.hpp"

#include <ranges>

namespace lf {

glob_path::glob_path(const std::initializer_list<element>& elements_init): elements(elements_init) {}

bool glob_path::matches(const fs_path& path) {
    return matches(path | std::views::transform([](const fs_path& p) { return p.string(); }));
}

}

