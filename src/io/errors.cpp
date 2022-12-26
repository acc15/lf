#include <iostream>

#include "errors.hpp"
#include "log.hpp"

namespace lf {

    const errors::end_t errors::end = {};

    errors::errors(const std::string& loc): _loc(loc), _count(0) {
    }

    errors::errors(const errors& c): _stream(c._stream.str()), _loc(c._loc), _count(c._count) {
    }

    errors::errors(errors&& c): _stream(std::move(c._stream)), _loc(std::move(c._loc)), _count(c._count) {
    }

    void errors::on_error(std::string_view msg) {
        if (!log.error()) {
            return;
        }
        
        std::ostream& out = log();
        if (!has_errors()) {
            out << "There are errors in " << _loc << std::endl << std::endl;
        }
        out << " - " << msg << std::endl;
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
        _stream.str("");
        _stream.clear();
        ++_count;
        return *this;
    }

}
