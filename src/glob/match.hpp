#pragma once

#include <concepts>
#include <ranges>
#include <variant>
#include <algorithm>

namespace lf {

struct globstar {};

template <std::ranges::range Sequence>
struct match_struct {
    std::ranges::const_iterator_t<const Sequence>& cur;
    const std::ranges::const_sentinel_t<const Sequence>& end;
};

template <typename T>
bool is_globstar(const T& v) {
    return std::holds_alternative<globstar>(v);
}

template <
    std::ranges::range Elements,
    std::ranges::range Sequence,
    std::invocable<
        std::ranges::range_const_reference_t<const Elements>, 
        match_struct<Sequence>&
    > MatchFn
>
bool glob_match(
    const Elements& elements, 
    const Sequence& sequence, 
    const MatchFn& try_match
) {

    using elem = std::ranges::range_value_t<const Elements>;
    using elem_iter = std::ranges::const_iterator_t<const Elements>;
    using seq_iter = std::ranges::const_iterator_t<const Sequence>;

    elem_iter e_cur = elements.begin();
    const elem_iter e_end = elements.end();

    seq_iter s_cur = sequence.begin();
    const seq_iter s_end = sequence.end();

    while (e_cur != e_end) {

        const elem_iter e_ns_begin = std::find_if_not(e_cur, e_end, &is_globstar<elem>);
        const elem_iter e_ns_end = std::find_if(e_ns_begin, e_end, &is_globstar<elem>);

        const bool has_star = e_ns_begin != e_cur;
        const bool last_chunk = e_ns_end == e_end;
        if (has_star && last_chunk && e_ns_begin == e_ns_end) {
            s_cur = s_end;
            return true;
        }

        match_struct<Sequence> m = { s_cur, s_end };
        while (true) {
            const seq_iter s_restore_cur = s_cur;

            bool ns_match = std::all_of(e_ns_begin, e_ns_end, [&m, &try_match](const auto& e) { 
                return m.cur != m.end && try_match(e, m);
            });

            if (!has_star) {
                if (!ns_match) {
                    return false;
                }
                break;
            }
            
            if (ns_match) {
                if (!last_chunk || s_cur == s_end) {
                    break;
                }
                continue;
            }

            s_cur = s_restore_cur;
            if (s_cur == s_end || !try_match(*e_cur, m)) {
                return false;
            }

        }
        e_cur = e_ns_end;

    }
    return s_cur == s_end;
}

}