#pragma once

#include "predicate.hpp"
#include <unordered_set>

namespace lf {

    template <encoding_type T>
    class glob<T>::set_predicate: public glob<T>::predicate {
        std::unordered_set<codepoint> set;
    public:
        set_predicate(std::unordered_set<codepoint>&& set): set(std::move(set)) {}
        bool test(const codepoint& v) const override {
            return set.contains(v);
        }
    };

}