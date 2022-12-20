
#include <string>
#include <string_view>
#include <unordered_map>

namespace leafsync {
    
    struct string_hash {
        using is_transparent = void;
        using hash_type = std::hash<std::string_view>;  // just a helper local type
        size_t operator()(std::string_view txt) const   { return hash_type{}(txt); }
        size_t operator()(const std::string& txt) const { return hash_type{}(txt); }
        size_t operator()(const char* txt) const        { return hash_type{}(txt); }
    };

    template<typename T>
    using unordered_string_map = std::unordered_map<std::string, T, string_hash, std::equal_to<>>;

}