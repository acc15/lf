#pragma once

#include <istream>
#include <vector>
#include <memory>

#include "../encoding/encoding.hpp"

namespace lf {

    template <encoding_type Encoding> 
    class glob {
    public:
        using encoding = Encoding;
        using char_type = typename encoding::char_type;
        using char_traits = typename encoding::char_traits;
        using string = typename encoding::string;
        using string_view = typename encoding::string_view;
        using codepoint = typename encoding::codepoint;

        using streambuf = std::basic_streambuf<char_type, char_traits>;
        using istreambuf_iterator = std::istreambuf_iterator<char_type, char_traits>;
        using istream = std::basic_istream<char_type, char_traits>;

        using pos_type = typename char_traits::pos_type;

        class matcher;
        class star_matcher;
        class char_matcher;
        class string_matcher;

        using matcher_ptr = std::unique_ptr<const matcher>;
        using matcher_vector = std::vector<matcher_ptr>;

    private:
        matcher_vector matchers;

    public:
        glob(matcher_vector&& matchers = {}): matchers(std::move(matchers)) {}

        bool matches(streambuf* buf) const {
            std::vector<std::pair<pos_type, size_t>> repetition_stack;

            return buf->sgetc() == char_traits::eof();
        }

        static glob parse(std::string_view view) {
            return glob();
        }

    };

}