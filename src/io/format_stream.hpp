#pragma once

#include <sstream>
#include <string>

namespace lf {
    struct format_stream: std::stringstream {
        operator std::string() const;
    };
}