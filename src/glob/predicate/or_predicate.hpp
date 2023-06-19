#pragma once

#include <algorithm>

#include "predicate.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::or_predicate: public glob<Encoding>::predicate {
        predicate_vector predicates;
    public:
        or_predicate(predicate_vector&& init): predicates(std::move(init)) {}
        
        bool test(const codepoint& v) const override {
            return std::any_of(predicates.begin(), predicates.end(), [&v](const predicate_ptr& p) { 
                return p->test(v); 
            });
        }
    };

}