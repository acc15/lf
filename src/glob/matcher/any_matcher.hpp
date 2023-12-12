#pragma once

#include "glob_matcher.hpp"

namespace lf {

class any_matcher: public glob_matcher {
public:
    bool matches(std::streambuf* buf, size_t, bool) const override;
};

}