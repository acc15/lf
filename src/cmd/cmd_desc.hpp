#pragma once

#include <vector>
#include <ostream>

namespace lf
{
    struct cmd_desc
    {
        std::vector<const char*> names;
        const char* parameters;
        const char* description;
    };

    std::ostream& operator<<(std::ostream& s, const cmd_desc& desc);
    
}
