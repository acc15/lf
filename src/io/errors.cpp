#include <fmt/core.h>

#include "errors.hpp"

namespace lf {

    errors::errors(const data_location& loc) : loc(loc), _count(0) {
    }

    void errors::operator()(std::string_view msg) {
        on_error(msg);
        ++_count;
    }

    void errors::on_error(std::string_view msg) {
        if (!has_errors()) {
            fmt::print("There are errors: {}\n\n", loc.source);
        }
        fmt::print(" - ");
        if (loc.line > 0) {
            fmt::print("Line {}, ", loc.line);
        }
        fmt::print("{}\n", msg);
    }

    size_t errors::error_count() const {
        return _count;
    }

    bool errors::has_errors() const {
        return _count > 0;
    }

}
