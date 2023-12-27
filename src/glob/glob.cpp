#include <utf8/unchecked.h>

#include "glob/glob.hpp"
#include "glob/match.hpp"
#include "glob/parse.hpp"
#include "glob/stringify.hpp"

namespace lf {

template <std::ranges::range Sequence>
struct match_visitor {

    match_struct<Sequence>& m;

    bool operator()(const glob::star&) {
        utf8::unchecked::next(m.cur);
        return true;
    }

    bool operator()(const glob::any&) {
        utf8::unchecked::next(m.cur);
        return true;
    }

    bool operator()(const glob::range& r) {
        const utf8::utfchar32_t cp = utf8::unchecked::next(m.cur);
        const auto it = r.map.upper_bound(cp);
        const bool in_range = it != r.map.begin() && cp <= std::prev(it)->second;
        return in_range != r.inverse;
    }

    bool operator()(const std::string& str) {
        const auto p = std::mismatch(str.begin(), str.end(), m.cur, m.end);
        m.cur = p.second;
        return p.first == str.end();
    }

};

glob::glob(const std::initializer_list<element>& v): elements(v) {}
glob::glob(const element_vector& v): elements(v) {}
glob::glob(element_vector&& v): elements(v) {}

bool glob::matches(std::string_view sv) const {
    return glob_match(elements, sv, [](const glob::element& e, match_struct<std::string_view>& m) {
        return std::visit(match_visitor {m}, e);
    });
}

std::string glob::stringify() const {
    return glob_stringify(elements);
}

glob glob::parse(std::string_view str) {
    auto it = str.begin();
    return glob_parse(it, str.end());
}

}