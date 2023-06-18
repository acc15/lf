#pragma once

#include "dynamic_predicate.hpp"

namespace lf {

    template <typename T>
    class any_predicate: public dynamic_predicate<T> {
    public:
        bool test(const T&) const override {
            return true;
        }
    };

}