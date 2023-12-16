#pragma once

#include <concepts>
#include <cstddef>
#include <vector>
#include <ranges>

namespace lf {

template <std::ranges::range Sequence>
struct match_struct {
    std::ranges::const_iterator_t<const Sequence>& cur;
    const std::ranges::const_sentinel_t<const Sequence>& end;
    std::size_t retry;
    const bool last;
};

template <
    std::ranges::range Elements,
    std::ranges::range Sequence,
    std::invocable<std::ranges::range_const_reference_t<const Elements>> RetryableFn,
    std::invocable<
        std::ranges::range_const_reference_t<const Elements>, 
        match_struct<Sequence>&
    > MatchFn
>
bool glob_match(
    const Elements& elements, 
    const Sequence& sequence, 
    const RetryableFn& is_star,
    const MatchFn& try_match
) {

    using elem_iter = std::ranges::const_iterator_t<const Elements>;
    using seq_iter = std::ranges::const_iterator_t<const Sequence>;
    
    struct retry_info {
        elem_iter e_it;
        seq_iter s_it;
        size_t retry;
    };

    std::vector<retry_info> retries;

    elem_iter e_cur = elements.begin();
    const elem_iter e_end = elements.end();

    seq_iter s_cur = sequence.begin();
    const seq_iter s_end = sequence.end();

    while (true) {

        if (e_cur != e_end) {

            const bool retryable = is_star(*e_cur);
            if (retryable && (retries.empty() || retries.back().e_it != e_cur)) {
                retries.push_back(retry_info { e_cur, s_cur, 0 });
            }
            
            const size_t retry = retryable ? retries.back().retry : 0;
            const bool last = (e_cur + 1 == e_end);
            
            match_struct<Sequence> m = { s_cur, s_end, retry, last };
            if (try_match(*e_cur, m)) {
                ++e_cur;
                continue;
            }

            if (retryable) {
                retries.pop_back();
            }

        } else if (s_cur == s_end) {
            return true;
        }

        if (retries.empty()) {
            return false;
        }

        retry_info& r = retries.back();
        e_cur = r.e_it;
        s_cur = r.s_it;
        ++r.retry;
        
    }

    return true;
}

}