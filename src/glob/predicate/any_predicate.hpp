#pragma once

#include "predicate.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::any_predicate: public glob<Encoding>::predicate {
    public:
        bool test(const codepoint&) const override {
            return true;
        }
    };

}