#include <algorithm>
#include <iterator>

#include "glob/matcher/string_matcher.hpp"

namespace lf {

string_matcher::string_matcher(std::string_view str): str(str) {}

bool string_matcher::matches(std::streambuf* buf, size_t, bool) const {
    return std::mismatch(
        str.begin(), str.end(), 
        std::istreambuf_iterator<char>(buf), std::istreambuf_iterator<char>()
    ).first == str.end();
}

}