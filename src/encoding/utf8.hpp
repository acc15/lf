#pragma once

#include "encoding.hpp"

#include <utf8cpp/utf8.h>

namespace lf {

    struct utf8_encoding {

        using codepoint = char32_t;
        using string = std::string;
        using string_view = std::string_view;
        using char_type = string::value_type;
        using char_traits = string::traits_type;

        template <typename OctetIterator>
        static codepoint next(OctetIterator& it) {
            return utf8::unchecked::next(it);
        }

    };

}