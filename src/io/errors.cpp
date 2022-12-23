#include <fmt/core.h>

#include "errors.hpp"

namespace leafsync {

    errors::errors(const data_location& loc) : loc(loc) {
    }

    void errors::operator()(std::string_view msg) {
        on_error(msg);
        ++_count;
    }

    void errors::on_error(std::string_view msg) {
        if (!has_errors()) {
            fmt::print("There are errors in file: {}\n\n", loc.source);
        }
        fmt::print(" - Line {}, {}\n", loc.line, msg);
    }

    size_t errors::error_count() const {
        return _count;
    }

    bool errors::has_errors() const {
        return _count > 0;
    }

}
