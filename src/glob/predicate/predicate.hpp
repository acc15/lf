#pragma once

#include "../globdef.hpp"

namespace lf {

    template <encoding_type T>
    class glob<T>::predicate {
    public:
        virtual bool test(const codepoint& v) const = 0;
    };

}