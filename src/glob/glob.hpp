#pragma once

#include <vector>
#include <map>
#include <variant>
#include <utf8/core.h>

namespace lf {

    class glob {
    public:
        using codepoint = utf8::utfchar32_t;
        
        struct star {};
        struct any {};
        struct range {
            std::map<codepoint, codepoint> map;
            bool inverse;

            range(const std::initializer_list<std::pair<const codepoint, codepoint>>& map_init);
        };

        using element = std::variant<star, any, range, std::string>;
        using element_vector = std::vector<element>;

        struct star_retryable_visitor {
            template<typename T>
            bool operator()(const T&) { return false; }
            bool operator()(const star&) { return true; }
        };

        element_vector elements;

        glob() = default;
        glob(const std::initializer_list<element>& v);

        bool matches(std::string_view sv) const;
    };

}