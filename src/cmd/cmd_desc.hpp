#pragma once

#include <span>
#include <ostream>

namespace lf
{
    struct cmd_desc
    {
        std::span<const char *> names;
        const char *description;
    };

    std::ostream& operator<<(std::ostream& s, const cmd_desc& desc);
    
}
