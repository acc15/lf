#pragma once

#include <string>
#include <istream>
#include <concepts>

namespace lf {

    template<typename T>
    concept encoding_type = requires {
        typename T::codepoint;
        typename T::char_type;
        typename T::char_traits;
        typename T::string;
    } && std::is_base_of_v<
        std::basic_string<typename T::char_type, typename T::char_traits>, 
        typename T::string
    > && requires (
        typename T::string::iterator& iter_ref
    ) {
        { T::next(iter_ref) } -> std::convertible_to<typename T::codepoint>;
    };

}