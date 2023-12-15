#pragma once

#include <concepts>
#include <cstddef>
#include <vector>
#include <ranges>

namespace lf {

template <
    std::ranges::range Elements,
    std::ranges::range Sequence,
    std::invocable<std::ranges::range_const_reference_t<const Elements>> RetryableFn,
    std::invocable<
        std::ranges::range_const_reference_t<const Elements>, 
        std::ranges::const_iterator_t<const Sequence>&,
        const std::ranges::const_sentinel_t<const Sequence>&, 
        std::size_t,
        bool
    > MatchFn
>
bool retryable_match(
    const Elements& elements, 
    const Sequence& sequence, 
    const RetryableFn& is_retryable,
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

            const bool retryable = is_retryable(*e_cur);
            if (retryable && (retries.empty() || retries.back().e_it != e_cur)) {
                retries.push_back(retry_info { e_cur, s_cur, 0 });
            }
            
            const size_t retry = retryable ? retries.back().retry : 0;
            const bool last = (e_cur + 1 == e_end);
            if (try_match(*e_cur, s_cur, s_end, retry, last)) {
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