#pragma once

#include <array>
#include <algorithm>
#include <string_view>
#include <charconv>
#include "util/string.hpp"

namespace lf {

    template <typename T, typename Names>
    T parse_enum(std::string_view str, const T& default_value, const Names& names_with_last_default) {
        std::string_view t = trim(str);
        
        const auto b = std::begin(names_with_last_default);
        const auto e = std::end(names_with_last_default) - 1;

        const auto found = std::find_first_of(b, e, &t, (&t)+1, &ci_str_eq);
        if (found != e) {
            return static_cast<T>(std::distance(b, found));
        }

        size_t index;
        const auto result = std::from_chars(t.begin(), t.end(), index);

        const size_t name_count = std::distance(b, e);
        if (result.ec == std::errc{} && index < name_count) {
            return static_cast<T>(index);
        }

        return default_value;
    }

    template <typename K, typename Values>
    const auto& pick_indexed(K index, const Values &values_with_last_default) {
        const size_t last_index = std::size(values_with_last_default) - 1;
        const size_t target_index = std::min(static_cast<size_t>(index), last_index);
        return values_with_last_default[target_index];
    }

}