#pragma once

#include <istream>
#include <vector>
#include <memory>

#include "../encoding/encoding.hpp"

namespace lf {

    template <encoding_type Encoding> 
    struct glob {

        using encoding = Encoding;
        using char_type = typename encoding::char_type;
        using char_traits = typename encoding::char_traits;
        using string = typename encoding::string;
        using string_view = typename encoding::string_view;
        using codepoint = typename encoding::codepoint;

        using streambuf = std::basic_streambuf<char_type, char_traits>;
        using istreambuf_iterator = std::istreambuf_iterator<char_type, char_traits>;
        using istream = std::basic_istream<char_type, char_traits>;

        class predicate;
        class set_predicate;
        class range_predicate;
        class or_predicate;
        class not_predicate;
        class any_predicate;

        class matcher;
        class star_matcher;
        class codepoint_matcher;
        class string_matcher;

        using predicate_ptr = std::unique_ptr<const predicate>;
        using predicate_vector = std::vector<predicate_ptr>;

        using matcher_ptr = std::unique_ptr<const matcher>;
        using matcher_vector = std::vector<matcher_ptr>;

    };

}