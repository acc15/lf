#pragma once

#include "../globdef.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::string_matcher: public glob<Encoding>::matcher {
        string str;
    public:
        string_matcher(std::string_view str): str(str) {}
        bool matches(streambuf* buf, size_t, bool) const override {
            return std::mismatch(
                str.begin(), str.end(), 
                istreambuf_iterator(buf), istreambuf_iterator()
            ).first == str.end();
        }
    };

}