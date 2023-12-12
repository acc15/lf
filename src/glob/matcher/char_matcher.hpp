#pragma once

#include <map>
#include <utf8/unchecked.h>

#include "glob_matcher.hpp"

namespace lf {

class char_matcher: public glob_matcher {
public:
    using codepoint = utf8::utfchar32_t;
    using range_map = std::map<codepoint, codepoint>;

private:
    range_map ranges;
    bool negate;

    range_map::iterator make_min_iter(const range_map::iterator& next, const codepoint& min, const codepoint& max);
    void add_minmax(const codepoint& min, const codepoint& max);

public:
    char_matcher(const range_map& ranges = {}, bool negate = false);

    void add(const codepoint& first, const codepoint& second);
    void add(const codepoint& v);
    bool test(const codepoint& v) const;
    bool matches(std::streambuf* buf, size_t, bool) const override;
};

}