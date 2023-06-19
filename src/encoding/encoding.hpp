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
        typename T::streambuf;
    } && std::is_base_of_v<std::basic_streambuf<typename T::char_type, typename T::char_traits>, typename T::streambuf>;

}