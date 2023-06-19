#pragma once

#include "matcher.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::string_matcher: public glob<Encoding>::matcher {
        string str;
    public:
        bool matches(streambuf& buf, size_t, bool) const override {
            return std::mismatch(
                str.begin(), str.end(), 
                istreambuf_iterator(buf), istreambuf_iterator()
            ).first == str.end();
        }
    };

}