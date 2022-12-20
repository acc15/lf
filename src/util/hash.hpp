#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace leafsync {
    
    struct str_hash {
        using is_transparent = void;
        using eq = std::equal_to<>;
        using impl = std::hash<std::string_view>;
        size_t operator()(std::string_view txt) const;
        size_t operator()(const std::string& txt) const;
        size_t operator()(const char* txt) const;
    };

}