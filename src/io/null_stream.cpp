#include "io/null_stream.hpp"

namespace lf {

    null_streambuf::int_type null_streambuf::overflow(null_streambuf::int_type c) {
        return c;
    }

    null_stream::null_stream(): std::ostream(&nullbuf) {
    }

    null_stream nullout;

}