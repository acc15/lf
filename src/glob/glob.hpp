#pragma once

#include <vector>
#include <memory>
#include <map>
#include <variant>

#include <utf8/core.h>

#include "matcher/glob_matcher.hpp"

namespace lf {

    class glob {
    public:
        using codepoint = utf8::utfchar32_t;
        
        struct star {};
        struct any {};
        struct range {
            bool inverse;
            std::map<codepoint, codepoint> map;
        };
        using string = std::string;

        using element = std::variant<star, any, range, string>;
        using elements = std::vector<element>;

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