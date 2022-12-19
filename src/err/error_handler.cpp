#include <fmt/core.h>

#include "error_handler.hpp"

namespace leafsync {

    error_handler::error_handler(std::string_view source, size_t line): source(source), line(line), has_errors(false) {
    }

    void error_handler::error(std::string_view msg) {
        if (!has_errors) {
            fmt::print("There are errors in file: {}\n\n", source);
            has_errors = true;
        }
        fmt::print(" - Line {}, {}\n", line, msg);
    }

}
