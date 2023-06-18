#pragma once

#include <string>
#include <utf8cpp/utf8.h>

namespace lf {

    struct utf8_traits {

        using codepoint = char32_t;
        using string = std::string;
        using string_view = std::string_view;
        using char_type = string::value_type;
        using char_traits = string::traits_type;

        static size_t length(const string& str) {
            return utf8::distance(str.begin(), str.end());
        }

        static size_t bytes(const string& str) {
            return sizeof(char_type) * str.size();
        }

        template <typename OctetIterator>
        static codepoint next(OctetIterator& it, const OctetIterator end) {
            return utf8::next(it, end);
        }

    };

}