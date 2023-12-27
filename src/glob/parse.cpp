#include "glob/parse.hpp"

namespace lf {

void glob_add_minmax(glob::range::map_type& ranges, utf8::utfchar32_t min, utf8::utfchar32_t max) {
    if (min > max) {
        std::swap(min, max);
    }
    
    const auto min_next = ranges.upper_bound(min);
    const auto min_iter = min_next != ranges.begin() && std::prev(min_next)->second + 1 >= min
        ? std::prev(min_next)
        : ranges.insert(min_next, { min, max });

    const auto max_next = ranges.upper_bound(max);
    if (max_next != ranges.end() && max + 1 == max_next->first) {
        min_iter->second = max_next->second;
        ranges.erase(min_next, std::next(max_next));
    } else {
        min_iter->second = std::max(max, std::prev(max_next)->second);
        ranges.erase(min_next, max_next);
    }
}

std::string& glob_make_string_ref(glob::element_vector& els) {
    if (els.empty() || !std::holds_alternative<std::string>(els.back())) {
        els.push_back(std::string());
    }
    return std::get<std::string>(els.back());
}


}