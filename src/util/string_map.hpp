#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <utility>

namespace lf {

    struct string_hash {
        using is_transparent = void;
        using hash_type = std::hash<std::string_view>;
        std::size_t operator()(std::string_view txt) const { return hash_type{}(txt); }
        std::size_t operator()(const std::string& txt) const { return hash_type{}(txt); }
        std::size_t operator()(const char* txt) const { return hash_type{}(txt); }
    };

    template <typename V, typename Alloc = std::allocator<std::pair<const std::string, V>>>
    using unordered_string_map = std::unordered_map<std::string, V, string_hash, std::equal_to<>, Alloc>;

}