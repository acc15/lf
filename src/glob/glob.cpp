#include <utf8/unchecked.h>

#include "glob/glob.hpp"
#include "glob/match.hpp"
#include "glob/parse.hpp"

namespace lf {

void glob::range::add(utf8::utfchar32_t min, utf8::utfchar32_t max) {
    if (min > max) {
        std::swap(min, max);
    }
    
    const auto min_next = map.upper_bound(min);
    const auto min_iter = min_next != map.begin() && std::prev(min_next)->second + 1 >= min
        ? std::prev(min_next)
        : map.insert(min_next, { min, max });

    const auto max_next = map.upper_bound(max);
    if (max_next != map.end() && max + 1 == max_next->first) {
        min_iter->second = max_next->second;
        map.erase(min_next, std::next(max_next));
    } else {
        min_iter->second = std::max(max, std::prev(max_next)->second);
        map.erase(min_next, max_next);
    }
}

void glob::range::add(utf8::utfchar32_t v) {
    add(v, v);
}

bool glob::range::contains(utf8::utfchar32_t v) const {
    const auto it = map.upper_bound(v);
    const bool in_range = it != map.begin() && v <= std::prev(it)->second;
    return in_range != inverse;
}

glob::glob(const std::initializer_list<element>& v): elements(v) {
}

glob::glob(const element_vector& v): elements(v) {
}

glob::glob(element_vector&& v): elements(v) {
}

bool glob::matches(std::string_view sv) const {
    struct match_visitor {
        match_struct<std::string_view>& m;

        bool operator()(const glob::star&) {
            utf8::unchecked::next(m.cur);
            return true;
        }

        bool operator()(const glob::any&) {
            utf8::unchecked::next(m.cur);
            return true;
        }

        bool operator()(const glob::range& r) {
            return r.contains(utf8::unchecked::next(m.cur));
        }

        bool operator()(const std::string& str) {
            const auto p = std::mismatch(str.begin(), str.end(), m.cur, m.end);
            m.cur = p.second;
            return p.first == str.end();
        }
    };
    return glob_match<match_visitor>(elements, sv);
}

glob glob::parse(std::string_view str) {
    auto it = str.begin();
    return glob(glob_parse(it, str.end()));
}

void glob::escape(std::string_view str, std::string& result) {
    auto it = str.begin();
    const auto end = str.end();
    while (it != end) {
        const auto it_start = it;
        const auto ch = utf8::unchecked::next(it);
        const bool escape = ch == U'*' || ch == U'?' || ch == U'[' || ch == U']';
        if (escape) {
            result.append(1, '[');
        }
        result.append(it_start, it);
        if (escape) {
            result.append(1, ']');
        }
    }
}

std::string glob::escape(std::string_view str) {
    std::string result;
    escape(str, result);
    return result;
}

void glob::stringify(std::string& result) const {
    struct stringify_visitor {
        std::string& result;
        
        void operator()(const std::string& str) {
            glob::escape(str, result); 
        }

        void operator()(const glob::star&) { 
            result.append(1, '*'); 
        }

        void operator()(const glob::any&) { 
            result.append(1, '?'); 
        }

        void operator()(const glob::range& range) {
            result.append(1, '[');
            if (range.inverse) {
                result.append(1, '!');
            }
            for (const auto& e: range.map) {
                if (e.first == e.second) {
                    utf8::unchecked::append(e.first, std::back_inserter(result));
                } else {
                    utf8::unchecked::append(e.first, std::back_inserter(result));
                    result.append(1, '-');
                    utf8::unchecked::append(e.second, std::back_inserter(result));
                }
            }
            result.append(1, ']');
        }
    } v = {result};
    for (const auto& e: elements) {
        std::visit(v, e); 
    }
}

std::string glob::stringify() const {
    std::string result;
    stringify(result);
    return result;
}

}