#pragma once

#include <ostream>
#include <streambuf>

namespace lf {

    struct null_streambuf: std::streambuf {
        int_type overflow(int_type c) override;
    };

    struct null_stream: std::ostream {
        null_stream();
        null_streambuf nullbuf;
    };

    extern null_stream nullout;

}