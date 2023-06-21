#pragma once

#include "matcher.hpp"
#include <map>

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::char_matcher: public glob<Encoding>::matcher {
    public:
        using map_type = std::map<codepoint, codepoint>;
        using map_iter = typename map_type::iterator;
        using map_citer = typename map_type::const_iterator;

        map_type map;
        bool negate;

        char_matcher(
            bool negate = false,
            const map_type& map = {}
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

        bool test(const codepoint& v) const {
            const map_citer it = map.upper_bound(v);
            const bool in_range = it != map.begin() && v <= std::prev(it)->second;
            return in_range != negate;
        }

        bool matches(streambuf* buf, size_t, bool) const override {
            istreambuf_iterator iter(buf), end;
            return iter != end && test(encoding::next(iter));
        }

    private:
        void add_minmax(const codepoint& min, const codepoint& max) {
            const map_iter min_next = map.upper_bound(min);
            const map_iter min_iter = (min_next != map.begin() && std::prev(min_next)->second + 1 >= min)
                ? std::prev(min_next)
                : map.insert(min_next, { min, max });

            const map_iter max_next = map.upper_bound(max);
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