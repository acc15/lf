#pragma once

#include <map>
#include <optional>

#include "predicate.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::class_predicate: public glob<Encoding>::predicate {
        using map_type = std::map<codepoint, codepoint>;
        map_type map;
        bool negate;

    public:
        class_predicate(
            const map_type& map, 
            bool negate = false
        ): map(map), negate(negate) {}

        bool test(const codepoint& v) const override {
            auto it = map.upper_bound(v);
            return (it != map.begin() && v <= (--it)->second) != negate;
        }
    };

}