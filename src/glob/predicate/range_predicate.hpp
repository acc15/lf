#pragma once

#include "predicate.hpp"

namespace lf {

    template <encoding_type T>
    class glob<T>::range_predicate: public glob<T>::predicate {
        codepoint min;
        codepoint max;
    public:
        range_predicate(const codepoint& min, const codepoint& max): min(min), max(max) {}
        bool test(const codepoint& v) const override {
            return v >= min && v <= max;
        }
    };

}