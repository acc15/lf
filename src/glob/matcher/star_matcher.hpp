#pragma once

#include "../globdef.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::star_matcher: public glob<Encoding>::matcher {
    public:
        bool is_repetitive() const override {
            return true;
        }

        bool matches(streambuf* buf, size_t repetition, bool is_last) const override {
            if (is_last) {
                return buf->pubseekoff(0, std::ios_base::end, std::ios_base::in) != -1;
            }
            if (repetition == 0) {
                return true;
            }
            for(istreambuf_iterator iter(buf), end; iter != end && repetition > 0;) {
                Encoding::next(iter);
                --repetition;
            }
            return repetition == 0;
        }
    };

}