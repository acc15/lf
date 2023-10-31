#include "util/string.hpp"

#include <algorithm>
#include <iterator>

namespace lf {

    bool is_ascii(char c) {
        return c >= 0;
    }

    char lower(char ch) {
        return is_ascii(ch) ? static_cast<char>(std::tolower(ch)) : ch;
    }

    char upper(char ch) {
        return is_ascii(ch) ? static_cast<char>(std::toupper(ch)) : ch;
    }

    bool is_whitespace(char ch) {
        return is_ascii(ch) && std::isspace(ch);
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
    size_t count_leading_whitespaces(const I first, const S last) {
        return std::distance(first, std::find_if_not(first, last, &is_whitespace));
    }

    std::string_view rtrim(std::string_view str) {
        str.remove_suffix(count_leading_whitespaces(str.rbegin(), str.rend()));
        return str;
    }

    std::string_view ltrim(std::string_view str) {
        str.remove_prefix(count_leading_whitespaces(str.begin(), str.end()));
        return str;
    }

    std::string_view trim(std::string_view str) {
        return ltrim(rtrim(str));
    }

    std::string lower(std::string_view str) {
        std::string result;
        result.reserve(str.size());
        std::transform(str.begin(), str.end(), std::back_inserter(result), static_cast<char (*)(char)>(&lower));
        return result;
    }

    std::string upper(std::string_view str) {
        std::string result;
        result.reserve(str.size());
        std::transform(str.begin(), str.end(), std::back_inserter(result), static_cast<char (*)(char)>(&upper));
        return result;
    }

    void lower(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), static_cast<char (*)(char)>(&lower));
    }

    void upper(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), static_cast<char (*)(char)>(&upper));
    }

    std::string lpad(std::string_view str, std::string::size_type len, char pad) {
        std::string result = static_cast<std::string>(str);
        if (result.size() < len) {
            result.insert(0, len - result.size(), pad);
        }
        return result;
    }

    bool ci_char_eq(char a, char b) {
        return upper(a) == upper(b);
    }

    bool ci_str_eq(std::string_view v1, std::string_view v2) {
        return v1.size() == v2.size() &&
            std::mismatch(v1.begin(), v1.end(), v2.begin(), &ci_char_eq).first == v1.end();
    }

}