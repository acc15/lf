#pragma once

#include <ostream>

#include "io/format.hpp"

namespace lf {

    struct openmode_format: format<false> {};

    std::ostream& operator<<(std::ostream& s, with_format<openmode_format, const std::ios_base::openmode> flags);

    template <typename Stream>
    struct default_openmode {
        static constexpr std::ios_base::openmode default_mode = static_cast<std::ios_base::openmode>(0);
        static constexpr std::ios_base::openmode force_mode = static_cast<std::ios_base::openmode>(0);
    };
    
    template <typename CharType, typename TraitsType>
    struct default_openmode<std::basic_istream<CharType, TraitsType>> {
        static constexpr std::ios_base::openmode default_mode = std::ios_base::in;
        static constexpr std::ios_base::openmode force_mode = std::ios_base::in;
    };

    template <typename CharType, typename TraitsType>
    struct default_openmode<std::basic_ostream<CharType, TraitsType>> {
        static constexpr std::ios_base::openmode default_mode = std::ios_base::out;
        static constexpr std::ios_base::openmode force_mode = std::ios_base::out;
    };

}