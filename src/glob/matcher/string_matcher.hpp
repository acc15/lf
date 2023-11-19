#pragma once

#include "glob_matcher.hpp"

namespace lf {

class string_matcher: public glob_matcher {
    std::string str;
public:
    string_matcher(std::string_view str);
    bool matches(std::streambuf* buf, size_t, bool) const override;
};

}