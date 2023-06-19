#pragma once

#include <memory>

namespace lf {

    template <typename T>
    class dynamic_predicate {
    public:
        virtual bool test(const T& v) const = 0;
    };

}