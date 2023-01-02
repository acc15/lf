#pragma once

#include <iterator>
#include <algorithm>

namespace lf {

    template <typename Compare>
    struct sorter {
        template <std::random_access_iterator It, std::sentinel_for<It> End>
        static void sort(It begin, const End end) {
            Compare cmp;
            std::sort(begin, end, cmp);
        }
    };

    template <>
    struct sorter<void> {
        template <std::random_access_iterator It, std::sentinel_for<It> End>
        static void sort(It, const End) {
        }
    };

}