#pragma once

#include <vector>
#include <algorithm>

#include "dynamic_predicate.hpp"

namespace lf {

    template <typename T>
    class or_predicate: public dynamic_predicate<T> {
        std::vector<dynamic_predicate_ptr<T>> predicates;
    public:
        or_predicate(std::vector<dynamic_predicate_ptr<T>>&& init): predicates(std::move(init)) {
        }
        
        bool test(const T& v) const override {
            return std::any_of(predicates.begin(), predicates.end(), [&v](const dynamic_predicate_ptr<T>& p) { 
                return p->test(v); 
            });
        }
    };

}