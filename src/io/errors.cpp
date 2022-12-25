#include <iostream>

#include "errors.hpp"

namespace lf {

    const errors::end_t errors::end = {};

    errors::errors(const data_location& loc) : loc(loc), _count(0) {
    }

    errors::errors(const errors& copy): loc(copy.loc), _count(copy._count), _stream(copy._stream.str()) {
    }
    
    errors::errors(errors&& move): loc(move.loc), _count(move._count), _stream(std::move(move._stream)) {
    }

    void errors::on_error(std::string_view msg) {
        if (!has_errors()) {
            std::cerr << "There are errors in " << loc.source << std::endl << std::endl;
        }
        std::cerr << " - ";
        if (loc.line > 0) {
            std::cerr << "Line " << loc.line << ", ";
        }
        std::cerr << msg << std::endl;
    }

    size_t errors::error_count() const {
        return _count;
    }

    bool errors::has_errors() const {
        return _count > 0;
    }

    template <>
    errors& errors::operator<<(const end_t&) {
        on_error(_stream.view());
        _stream.clear();
        ++_count;
        return *this;
    }

}
