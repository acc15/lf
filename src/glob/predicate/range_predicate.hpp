#pragma once

#include "dynamic_predicate.hpp"

namespace lf {

    template <typename T>
    class range_predicate: public dynamic_predicate<T> {
        T min;
        T max;
    public:
        range_predicate(const T& min, const T& max): min(min), max(max) {}
        bool test(const T& v) const override {
            return v >= min && v <= max;
        }
    };

}