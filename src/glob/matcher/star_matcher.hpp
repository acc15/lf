#pragma once

#include "matcher.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::star_matcher: public glob<Encoding>::matcher {
    public:
        bool is_repetitive() const override {
            return true;
        }

        bool matches(streambuf& buf, size_t repetition, bool is_last) const override {
            if (is_last) {
                buf.pubseekoff(0, std::ios_base::end); // consume all chars past the end of stream
                return true;
            }
            return buf.pubseekoff(repetition, std::ios_base::cur) != -1;
        }
    };

}