#pragma once

#include <concepts>
#include <vector>
#include <map>
#include <variant>
#include <utf8/unchecked.h>

namespace lf {

    class glob {
    public:
        struct any {};
        struct star {};
        struct range {
            using map_type = std::map<utf8::utfchar32_t, utf8::utfchar32_t>;
            map_type map;
            bool inverse = false;
        };

        template <typename T>
        static bool is_star(const T& v) {
            return std::holds_alternative<star>(v);
        }

        using element = std::variant<std::string, range, any, star>;
        using element_vector = std::vector<element>;

        element_vector elements;

        glob() = default;
        glob(const element_vector& v);
        glob(element_vector&& v);
        glob(const std::initializer_list<element>& v);

        bool matches(std::string_view sv) const;
        static glob parse(std::string_view str);

    };

}