#include <iterator>
#include <utf8/unchecked.h>

#include "any_matcher.hpp"

namespace lf {

bool any_matcher::matches(std::streambuf* buf, size_t, bool) const {
    std::istreambuf_iterator<char> iter(buf), end;
    if (iter == end) {
        return false;
    }
    utf8::unchecked::next(iter);
    return true;
}

}