#include "null_stream.hpp"
#include <ostream>

namespace lf {

    null_stream::null_streambuf::int_type null_stream::null_streambuf::overflow(null_stream::null_streambuf::int_type c) {
        return c;
    }

    null_stream::null_stream(): std::ostream(&nullbuf) {
    }

    null_stream nullout;

}