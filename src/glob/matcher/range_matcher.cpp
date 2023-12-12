#include <iterator>
#include "glob/matcher/range_matcher.hpp"

namespace lf {

range_matcher::range_matcher(
    const range_map& ranges,
    bool negate
): ranges(ranges), negate(negate) {}

void range_matcher::add(const codepoint& first, const codepoint& second) {
    if (first <= second) {
        add_minmax(first, second);
    } else {
        add_minmax(second, first);
    }
}

void range_matcher::add(const codepoint& v) {
    add_minmax(v, v);
}

bool range_matcher::test(const codepoint& v) const {
    const range_map::const_iterator it = ranges.upper_bound(v);
    const bool in_range = it != ranges.begin() && v <= std::prev(it)->second;
    return in_range != negate;
}

bool range_matcher::matches(std::streambuf* buf, size_t, bool) const {
    std::istreambuf_iterator<char> iter(buf), end;
    return iter != end && test(utf8::unchecked::next(iter));
}

range_matcher::range_map::iterator range_matcher::make_min_iter(
    const range_map::iterator& next, 
    const codepoint& min, 
    const codepoint& max
) {
    if (next != ranges.begin()) {
        const range_map::iterator prev = std::prev(next);
        if (prev->second + 1 >= min) {
            return prev;
        }
    }
    return ranges.insert(next, { min, max });
}

void range_matcher::add_minmax(const codepoint& min, const codepoint& max) {
    const auto min_next = ranges.upper_bound(min);
    const auto min_iter = make_min_iter(min_next, min, max);

    const auto max_next = ranges.upper_bound(max);
    if (max_next != ranges.end() && max + 1 == max_next->first) {
        min_iter->second = max_next->second;
        ranges.erase(min_next, std::next(max_next));
    } else {
        min_iter->second = std::max(max, std::prev(max_next)->second);
        ranges.erase(min_next, max_next);
    }
}


}
