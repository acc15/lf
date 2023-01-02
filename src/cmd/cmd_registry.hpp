#pragma once

#include <ostream>
#include <unordered_map>
#include <string_view>
#include <span>

#include "cmd.hpp"

namespace lf
{

    class cmd_registry {
    public:
        cmd_registry();
        int run(std::span<const char*> args) const;
        static const cmd* const list[];
    private:
        std::unordered_map<std::string_view, const cmd*> _names;
    };

    std::ostream& operator<<(std::ostream& s, const cmd_registry& r);

    extern const cmd_registry cmds;

}
