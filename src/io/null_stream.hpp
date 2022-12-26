#include <ostream>
#include <streambuf>

namespace lf {

    struct null_stream: std::ostream {
        null_stream();
        struct null_streambuf: std::streambuf {
            int_type overflow(int_type c) override;
        };
        null_streambuf nullbuf;
    };

    extern null_stream nullout;

}