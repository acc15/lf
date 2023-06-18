#pragma once

#include <memory>

#include "dynamic_predicate.hpp"

namespace lf {

    template <typename T>
    class not_predicate: public dynamic_predicate<T> {
        dynamic_predicate_ptr<T> predicate;
    public:
        not_predicate(dynamic_predicate_ptr<T> predicate): predicate(std::move(predicate)) {}
        bool test(const T& v) const override {
            return !predicate->test(v);
        }
    };

}