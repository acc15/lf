#pragma once

#include "matcher.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::codepoint_matcher: public glob<Encoding>::matcher {
        predicate_ptr predicate;
    public:
        bool matches(streambuf& buf, size_t, bool) const override {
            istreambuf_iterator iter(buf), end;
            return iter != end && predicate->matches(encoding::next(iter, end));
        }
    };

}