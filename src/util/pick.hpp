#pragma once

#include <array>
#include <algorithm>

namespace leafsync {

    template <typename T, size_t N>
    const T& pick_indexed_bool(bool flag, const T (&values_with_last_default)[N]) {
        static_assert(N == 2, "illegal values_with_last_default size (for bool values it must be exactly 2)");
        return values_with_last_default[static_cast<size_t>(flag)];
    }

    template <typename K, typename T, size_t N>
    const T& pick_indexed_with_last_default(K index, const T (&values_with_last_default)[N]) {
        size_t target_index = std::min(static_cast<size_t>(index), std::size(values_with_last_default) - 1);
        return values_with_last_default[target_index];
    }

}