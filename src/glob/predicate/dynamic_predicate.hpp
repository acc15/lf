#pragma once

#include <memory>

namespace lf {

    template <typename T>
    class dynamic_predicate {
    public:
        virtual bool test(const T& v) const = 0;
    };

    template <typename T>
    using dynamic_predicate_ptr = std::unique_ptr<const dynamic_predicate<T>>;

}