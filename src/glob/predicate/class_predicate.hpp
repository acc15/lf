#pragma once

#include <map>
#include <optional>

#include "predicate.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::class_predicate: public glob<Encoding>::predicate {
    public:
        using map_type = std::map<codepoint, codepoint>;
        using map_iter = typename map_type::iterator;

        map_type map;
        bool negate;

        class_predicate(
            const map_type& map = {}, 
            bool negate = false
        ): map(map), negate(negate) {}

        bool add(const codepoint& first, const codepoint& second) {
            return first <= second 
                ? add_minmax(first, second) 
                : add_minmax(second, first);
        }

        bool add(const codepoint& v) {
            map_iter begin = map.begin(), end = map.end(), next = map.upper_bound(v);
            if (next != begin) { 
                map_iter prev = std::prev(next);
                if (v <= prev->second) {
                    return false; // already in range
                }
                if (prev->second + 1 == v) {
                    if (next != end && v + 1 == next->first) {
                        prev->second = next->second;
                        map.erase(next);
                    } else {
                        prev->second = v;
                    }
                    return true;
                }
            }
            if (next != end && v + 1 == next->first) {
                map.insert(next, { v, next->second });
                map.erase(next);
            } else {
                map.insert(next, { v, v });
            }
            return true;
        }

        bool test(const codepoint& v) const override {
            auto it = map.upper_bound(v);
            return (it != map.begin() && v <= (--it)->second) != negate;
        }

    private:
        bool add_minmax(const codepoint& min, const codepoint& max) {
            return false;
        }
    };

}