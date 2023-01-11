#pragma once

#include <string_view>
#include <cstddef>

namespace lf {

    struct opt {
        constexpr static std::size_t UNBOUNDED = static_cast<std::size_t>(-1);

        std::string_view    name;
        char                alias = '\0';
        std::string_view    description;
        std::string_view    arg_description = "";
        std::size_t         min_occurs = 0;
        std::size_t         max_occurs = 0;
    };

}


