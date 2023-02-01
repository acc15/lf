#pragma once

#include <ostream>

#include "io/format.hpp"

namespace lf {

    struct ios_flags_format: format<false> {};

    std::ostream& operator<<(std::ostream& s, with_format<ios_flags_format, const std::ios_base::openmode> flags);

}