#pragma once

#include <string>
#include <istream>
#include <concepts>

namespace lf {

    template<typename T>
    concept encoding_type = requires
    {
        typename T::codepoint;
        typename T::char_type;
        typename T::char_traits;
    };

}