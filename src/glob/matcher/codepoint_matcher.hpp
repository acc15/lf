#pragma once

#include "matcher.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::codepoint_matcher: public glob<Encoding>::matcher {
        predicate_ptr predicate;
    public:
        codepoint_matcher(predicate_ptr&& predicate): predicate(std::move(predicate)) {}
        bool matches(streambuf* buf, size_t, bool) const override {
            istreambuf_iterator iter(buf), end;
            return iter != end && predicate->test(encoding::next(iter));
        }
    };

}