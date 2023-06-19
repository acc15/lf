#pragma once

#include <memory>

#include "dynamic_predicate.hpp"

namespace lf {

    template <typename T>
    class not_predicate: public dynamic_predicate<T> {
        std::unique_ptr<const dynamic_predicate<T>> predicate;
    public:
        not_predicate(std::unique_ptr<const dynamic_predicate<T>>&& predicate): predicate(std::move(predicate)) {}
        bool test(const T& v) const override {
            return !predicate->test(v);
        }
    };

}