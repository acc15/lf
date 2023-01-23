#pragma once

#include <sstream>
#include <string>

namespace lf {
    struct format_stream: std::stringstream {
        using std::stringstream::stringstream;
        operator std::string() const;
    };
}