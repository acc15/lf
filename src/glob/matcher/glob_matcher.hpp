#pragma once

#include <streambuf>

namespace lf {

class glob_matcher {
public:
    virtual bool is_repetitive() const;
    virtual bool matches(std::streambuf* buf, std::size_t repetition, bool is_last) const = 0;
};

}