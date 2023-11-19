#include <utf8/unchecked.h>

#include "star_matcher.hpp"

namespace lf {

bool star_matcher::is_repetitive() const {
    return true;
}

bool star_matcher::matches(std::streambuf* buf, size_t repetition, bool is_last) const {
    if (is_last) {
        return buf->pubseekoff(0, std::ios_base::end, std::ios_base::in) != -1;
    }
    if (repetition == 0) {
        return true;
    }
    for(std::istreambuf_iterator<char> iter(buf), end; iter != end && repetition > 0;) {
        utf8::unchecked::next(iter);
        --repetition;
    }
    return repetition == 0;
}

}