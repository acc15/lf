#include <utf8/unchecked.h>

#include "glob/glob.hpp"
#include "glob/match.hpp"

namespace lf {

template <std::ranges::range Sequence>
struct match_visitor {

    match_struct<Sequence>& m;

    bool operator()(const glob::any&) {
        if (m.cur == m.end) {
            return false;
        }
        utf8::unchecked::next(m.cur);
        return true;
    }

    bool operator()(const glob::range& r) {
        if (m.cur == m.end) {
            return false;
        }
        const utf8::utfchar32_t cp = utf8::unchecked::next(m.cur);
        const auto it = r.map.upper_bound(cp);
        const bool in_range = it != r.map.begin() && cp <= std::prev(it)->second;
        return in_range != r.inverse;
    }

    bool operator()(const std::string& str) {
        const auto p = std::mismatch(str.begin(), str.end(), m.cur, m.end);
        if (p.first != str.end()) {
            return false;
        }
        m.cur = p.second;
        return true;
    }

    bool operator()(const glob::star&) {
        if (m.last) {
            m.cur = m.end;
        } else {
            for (; m.cur != m.end && m.retry > 0; utf8::unchecked::next(m.cur), --m.retry);
        }
        return m.retry == 0;
    }
};

glob::range::range(const std::initializer_list<std::pair<const codepoint, codepoint>>& map_init): 
    map(map_init), 
    inverse(false) {
}

glob::glob(const std::initializer_list<element>& v): elements(v) {
}

bool glob::matches(std::string_view sv) const {
    return glob_match(elements, sv, [](const glob::element& e) {
        return std::visit([](const auto& v) { return std::is_same_v<std::decay_t<decltype(v)>, glob::star>; }, e);
    }, [](const glob::element& e, match_struct<std::string_view>& m) {
        return std::visit(match_visitor {m}, e);
    });
}

}