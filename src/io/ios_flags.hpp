#pragma once

#include <ostream>

#include "io/format.hpp"

namespace lf {

    std::ostream& operator<<(std::ostream& s, with_format<format::TEXT, const std::ios_base::openmode&> flags);

}