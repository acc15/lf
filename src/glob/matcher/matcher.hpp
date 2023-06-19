#pragma once

#include <cstddef>
#include "../../encoding/encoding.hpp"

namespace lf {

    template <encoding_type Encoding>
    class matcher {
    public:
        virtual bool is_repetitive() const {
            return false;
        }

        virtual bool matches(std::basic_streambuf<typename Encoding::char_type, typename Encoding::char_traits>& buf, size_t repetition, bool is_last) const = 0;
    };

}