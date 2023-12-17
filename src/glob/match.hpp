#pragma once

#include <concepts>
#include <cstddef>
#include <ranges>
#include <variant>
#include <algorithm>

namespace lf {

struct globstar {};

template <std::ranges::range Sequence>
struct match_struct {
    std::ranges::const_iterator_t<const Sequence>& cur;
    const std::ranges::const_sentinel_t<const Sequence>& end;
    const bool last;
};

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

    using elem_iter = std::ranges::const_iterator_t<const Elements>;
    using seq_iter = std::ranges::const_iterator_t<const Sequence>;
    
    elem_iter e_cur = elements.begin();
    const elem_iter e_end = elements.end();

    seq_iter s_cur = sequence.begin();
    const seq_iter s_end = sequence.end();

    while (e_cur != e_end) {

        const auto e_ns_begin = std::find_if_not(e_cur, e_end, [](const auto& el) { 
            return std::holds_alternative<globstar>(el); 
        });

        const auto e_ns_end = std::find_if(e_ns_begin, e_end, [](const auto& el) { 
            return std::holds_alternative<globstar>(el); 
        });

        const bool has_star = e_ns_begin != e_cur;        
        while (true) {

            const auto s_restore_cur = s_cur;

            bool ns_match = true;
            for (auto e_ns_cur = e_ns_begin; e_ns_cur != e_ns_end; ++e_ns_cur) {
                match_struct<Sequence> m = { s_cur, s_end, e_ns_cur + 1 == e_end };
                if (!try_match(*e_ns_cur, m)) {
                    ns_match = false;
                    break;
                }
            }

            if (!has_star) {
                if (!ns_match) {
                    return false;
                }
                break;
            }
            
            bool last_chunk = e_ns_end == e_end;
            if (ns_match && (!last_chunk || s_cur == s_end)) {
                break;
            }

            s_cur = s_restore_cur;

            match_struct<Sequence> star_match = { s_cur, s_end, e_ns_begin == e_ns_end };
            if (!try_match(*e_cur, star_match)) {
                return false;
            }

        }
        e_cur = e_ns_end;

    }
    return s_cur == s_end;
}

}