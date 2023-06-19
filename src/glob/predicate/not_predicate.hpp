#pragma once

#include "predicate.hpp"

namespace lf {

    template <encoding_type Encoding>
    class glob<Encoding>::not_predicate: public glob<Encoding>::predicate {
        predicate_ptr predicate;
    public:
        not_predicate(predicate_ptr&& predicate): predicate(std::move(predicate)) {}
        bool test(const codepoint& v) const override {
            return !predicate->test(v);
        }
    };

}