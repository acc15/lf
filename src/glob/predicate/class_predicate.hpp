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

        bool add(const codepoint& first, const codepoint& second) {
            return first <= second 
                ? add_minmax(first, second) 
                : add_minmax(second, first);
        }

        bool add(const codepoint& v) {
            const map_iter begin = map.begin(), end = map.end();

            const map_iter next = map.upper_bound(v);
            if (next != begin) { 
                const map_iter prev = std::prev(next);
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
            const map_citer it = map.upper_bound(v);
            const bool in_range = it != map.begin() && v <= std::prev(it)->second;
            return in_range != negate;
        }

    private:
        bool add_minmax(const codepoint& min, const codepoint& max) {

            const map_iter begin = map.begin(), end = map.end();
            
            const map_iter max_next = map.upper_bound(max);
            if (max_next == begin) {
                if (max_next != end && max + 1 == max_next->first) {
                    map.insert(begin, { min, max_next->second });
                    map.erase(max_next);
                } else {
                    map.insert(begin, { min, max });
                }
                return true;
            }

            // no empty map here
            
            const map_iter min_next = map.upper_bound(min);
            if (min_next == end) {
                const map_iter min_prev = std::prev(min_next);
                if (min - 1 <= min_prev->second) {
                    min_prev->second = max;
                } else {
                    map.insert(end, { min, max });
                }
                return true;
            }

            return false;
        }
    };

}