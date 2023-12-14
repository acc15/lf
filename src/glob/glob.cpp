#include <utf8/unchecked.h>

#include "glob/glob.hpp"
#include "glob/match.hpp"

namespace lf {

template <typename SequenceIter>
struct glob_match_visitor {

    SequenceIter& begin;
    const SequenceIter& end;
    size_t repetition;
    const bool last;

    bool operator()(const glob::any&) {
        if (begin == end) {
            return false;
        }
        utf8::unchecked::next(begin);
        return true;
    }

    bool operator()(const glob::range& r) {
        if (begin == end) {
            return false;
        }
        const utf8::utfchar32_t cp = utf8::unchecked::next(begin);
        const auto it = r.map.upper_bound(cp);
        const bool in_range = it != r.map.begin() && cp <= std::prev(it)->second;
        return in_range != r.inverse;
    }

    bool operator()(const glob::string& str) {
        const auto p = std::mismatch(str.begin(), str.end(), begin, end);
        if (p.first != str.end()) {
            return false;
        }
        begin = p.second;
        return true;
    }

    bool operator()(const glob::star&) {
        if (last) {
            begin = end;
        } else {
            for (; begin != end && repetition > 0; utf8::unchecked::next(begin), --repetition);
        }
        return repetition == 0;
    }
};

glob::range::range(const std::initializer_list<std::pair<const codepoint, codepoint>>& map_init): 
    map(map_init), 
    inverse(false) {
}

glob::glob(const std::initializer_list<element>& v): elements(v) {
}

bool glob::matches(std::string_view sv) const {
    return retryable_match(elements, sv, [](const glob::element& e) {
        return std::visit([](auto&& v) { return std::is_same_v<std::decay_t<decltype(v)>, glob::star>; }, e);
    }, [](const glob::element& e, auto& begin, const auto& end, size_t repetition, bool last) {
        return std::visit(glob_match_visitor {begin, end, repetition, last}, e);
    });
}

}