#pragma once

#include <unordered_set>

#include "dynamic_predicate.hpp"

namespace lf {

    template <typename T>
    class set_predicate: public dynamic_predicate<T> {
        std::unordered_set<T> set;
    public:
        set_predicate(std::unordered_set<T>&& set): set(std::move(set)) {}
        bool test(const T& v) const override {
            return set.contains(v);
        }
    };

}