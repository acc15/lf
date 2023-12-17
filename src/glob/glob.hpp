#pragma once

#include <vector>
#include <map>
#include <variant>
#include <utf8/core.h>
#include "glob/match.hpp"

namespace lf {

    class glob {
    public:
        struct any {};
        struct range {
            std::map<utf8::utfchar32_t, utf8::utfchar32_t> map;
            bool inverse;

            range(const std::initializer_list<std::pair<const utf8::utfchar32_t, utf8::utfchar32_t>>& map_init);
        };

        using element = std::variant<globstar, any, range, std::string>;
        using element_vector = std::vector<element>;

        element_vector elements;

        glob() = default;
        glob(const std::initializer_list<element>& v);

        bool matches(std::string_view sv) const;
    };

}