#include "io/util.hpp"

#include <cstring>

using iob = std::ios_base;

namespace lf {

    std::istream& reset_fail_on_eof(std::istream& s) {
        iob::iostate state = s.rdstate();
        if (state == (iob::eofbit | iob::failbit)) {
            s.clear(state & ~iob::failbit);
        }
        return s;
    }

    const char* get_errno_message() {
        return std::strerror(errno);
    }

}