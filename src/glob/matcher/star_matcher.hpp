#pragma once

#include "glob_matcher.hpp"

namespace lf {

class star_matcher: public glob_matcher {
public:
    bool is_repetitive() const override;
    bool matches(std::streambuf* buf, size_t repetition, bool is_last) const override;
};

}