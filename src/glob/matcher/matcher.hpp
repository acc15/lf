#pragma once

#include "../globdef.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::matcher {
    public:
        virtual bool is_repetitive() const {
            return false;
        }

        virtual bool matches(streambuf& buf, size_t repetition, bool is_last) const = 0;
    };

}