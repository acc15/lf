#pragma once

#include <unordered_map>
#include <string_view>

#include "cmd.hpp"

namespace leafsync
{
    
class cmd_registry {
public:
    cmd_registry();
    std::string usage() const;

    int run(std::span<const char*> args) const;

private:
    std::unordered_map<std::string_view, const cmd*> _names;
    static const cmd* const _list[];
};

extern const cmd_registry cmds;

}
