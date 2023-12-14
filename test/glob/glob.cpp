#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "glob/glob.hpp"
#include "glob/matcher/any_matcher.hpp"
#include "glob/matcher/range_matcher.hpp"
#include "glob/matcher/star_matcher.hpp"
#include "glob/matcher/string_matcher.hpp"
#include "test_util.hpp"

using namespace lf;

class match_tester {
public:
    std::stringstream stream;
    std::streambuf* buf;

    match_tester(const std::string& string):
        stream(string),
        buf(stream.rdbuf())
    {}

    std::size_t pos() const {
        return static_cast<std::size_t>(buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
    }

    std::size_t size() const {
        return stream.view().size();
    }

};

TEST_CASE("glob: matches", "[glob]") {

    glob g(make_unique_container<glob::matcher_vector>(
        std::make_unique<any_matcher>(),
        std::make_unique<range_matcher>(range_matcher::range_map {{U'A', U'Z'}}, false), // A-Z
        std::make_unique<star_matcher>(), // zero or more chars
        std::make_unique<string_matcher>(".txt") // .txt
    ));

    match_tester t1("ЭZany12.txt");
    REQUIRE( g.matches(t1.buf) );
    REQUIRE( t1.pos() == t1.size() );

    match_tester t2("ZnotnextZ.txt");
    REQUIRE_FALSE( g.matches(t2.buf) );
    REQUIRE( t2.pos() == 2 );

}

template <typename SequenceIter>
struct glob_match_visitor {

    SequenceIter& begin;
    const SequenceIter& end;
    size_t repetition;
    const bool last;

    bool operator()(const glob::any&) {
        if (begin == end) {
            return false;
        }
        utf8::unchecked::next(begin);
        return true;
    }

    bool operator()(const glob::range& r) {
        if (begin == end) {
            return false;
        }
        const utf8::utfchar32_t cp = utf8::unchecked::next(begin);
        const auto it = r.map.upper_bound(cp);
        const bool in_range = it != r.map.begin() && cp <= std::prev(it)->second;
        return in_range != r.inverse;
    }

    bool operator()(const glob::string& str) {
        const auto p = std::mismatch(str.begin(), str.end(), begin, end);
        if (p.first != str.end()) {
            return false;
        }
        begin = p.second;
        return true;
    }

    bool operator()(const glob::star&) {
        if (last) {
            begin = end;
        } else {
            for (; begin != end && repetition > 0; utf8::unchecked::next(begin), --repetition);
        }
        return repetition == 0;
    }
};

template <
    typename Elements,
    typename Sequence,
    std::invocable<typename Elements::const_reference> RetryableFn,
    std::invocable<
        typename Elements::const_reference, 
        typename Sequence::const_iterator&, 
        const typename Sequence::const_iterator&,
        size_t,
        bool
    > MatchFn
>
bool retryable_match(
    const Elements& elements, 
    const Sequence& sequence, 
    const RetryableFn& is_retryable,
    const MatchFn& try_match
) {

    using elem_iter = typename Elements::const_iterator;
    using seq_iter = typename Sequence::const_iterator;
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

bool glob_match(const glob::elements& els, std::string_view sv) {
    return retryable_match(els, sv, [](const glob::element& e) {
        return std::visit([](auto&& v) { return std::is_same_v<std::decay_t<decltype(v)>, glob::star>; }, e);
    }, [](const glob::element& e, auto& begin, const auto& end, size_t repetition, bool last) {
        return std::visit(glob_match_visitor {begin, end, repetition, last}, e);
    });
}

TEST_CASE("glob: retryable_match", "[glob]") {

    REQUIRE( glob_match({
        glob::star {},
        glob::string("abc"),
        glob::any {}
    }, "xaaabcaabcx") );

    REQUIRE_FALSE( glob_match({
        glob::star {},
        glob::string("abc")
    }, "xaaabcaabcx") );

}

