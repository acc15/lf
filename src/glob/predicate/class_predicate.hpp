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
        using map_citer = typename map_type::const_iterator;

        map_type map;
        bool negate;

        class_predicate(
            const map_type& map = {}, 
            bool negate = false
        ): map(map), negate(negate) {}

        void add(const codepoint& first, const codepoint& second) {
            if (first <= second) {
                add_minmax(first, second);
            } else {
                add_minmax(second, first);
            }
        }

        void add(const codepoint& v) {
            add_minmax(v, v);
        }

        bool test(const codepoint& v) const override {
            const map_citer it = map.upper_bound(v);
            const bool in_range = it != map.begin() && v <= std::prev(it)->second;
            return in_range != negate;
        }

    private:
        void add_minmax(const codepoint& min, const codepoint& max) {
            const map_iter min_next = map.upper_bound(min);
            const map_iter min_iter = (min_next != map.begin() && std::prev(min_next)->second + 1 >= min)
                ? std::prev(min_next)
                : map.insert(min_next, { min, max });

            const map_iter max_next = map.upper_bound(max);
            
            map_iter max_iter;
            if (max_next != map.end() && max + 1 == max_next->first) {
                min_iter->second = max_next->second;
                map.erase(min_next, std::next(max_next));
            } else {
                min_iter->second = std::max(max, std::prev(max_next)->second);
                map.erase(min_next, max_next);
            }
        }
    };

}