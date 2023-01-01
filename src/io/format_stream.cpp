#include "io/format_stream.hpp"

namespace lf {

    format_stream::operator std::string() const {
        return str();
    }

}