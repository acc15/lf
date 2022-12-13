// #include <unordered_map>
#include <span>

#include "cmd.hpp"

namespace leafsync
{
    
class cmd_registry {
public:
    cmd_registry(const std::span<const cmd*>& cmds);
    std::span<const cmd*> cmds;

    std::string usage() const;
    // std::unordered_map<const char*, const cmd*> _names;

};

extern const cmd_registry cmds;

}
