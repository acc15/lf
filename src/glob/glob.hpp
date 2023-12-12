#pragma once

#include <vector>
#include <memory>

#include "matcher/glob_matcher.hpp"

namespace lf {

    class glob {
    public:
        using matcher_ptr = std::unique_ptr<const glob_matcher>;
        using matcher_vector = std::vector<matcher_ptr>;

    private:
        matcher_vector matchers;

        struct repetition_info {
            std::streampos position;
            size_t index;
            size_t repetition;
        };

    public:
        glob(matcher_vector&& matchers = {});
        bool matches(std::streambuf* buf) const;
    };

}