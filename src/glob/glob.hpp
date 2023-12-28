#pragma once

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

            void add(utf8::utfchar32_t min, utf8::utfchar32_t max);
            void add(utf8::utfchar32_t v);
            bool contains(utf8::utfchar32_t v) const;

        };

        using element = std::variant<std::string, range, any, star>;
        using element_vector = std::vector<element>;

        element_vector elements;

        glob() = default;
        glob(const element_vector& v);
        glob(element_vector&& v);
        glob(const std::initializer_list<element>& v);

        bool matches(std::string_view sv) const;
        
        void stringify(std::string& result) const;
        std::string stringify() const;
        
        static void escape(std::string_view str, std::string& result);
        static std::string escape(std::string_view str);

        static glob parse(std::string_view str);

    };

}