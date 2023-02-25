#pragma once

#include <iostream>
#include <concepts>

#include "io/format.hpp"
#include "io/stream_type.hpp"

namespace lf {

    struct openmode_format: format<false> {};

    std::ostream& operator<<(std::ostream& s, with_format<openmode_format, const std::ios_base::openmode> flags);

    template <typename Stream>
    struct default_openmode {
        static constexpr std::ios_base::openmode default_mode = static_cast<std::ios_base::openmode>(0);
        static constexpr std::ios_base::openmode force_mode = static_cast<std::ios_base::openmode>(0);
    };

    template <istream_type Stream>
    struct default_openmode<Stream> {
        static constexpr std::ios_base::openmode default_mode = std::ios_base::in;
        static constexpr std::ios_base::openmode force_mode = std::ios_base::in;
    };

    template <ostream_type Stream>
    struct default_openmode<Stream> {
        static constexpr std::ios_base::openmode default_mode = std::ios_base::out;
        static constexpr std::ios_base::openmode force_mode = std::ios_base::out;
    };

}