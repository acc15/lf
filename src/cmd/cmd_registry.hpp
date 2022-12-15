#include <string>
#include <unordered_map>

#include "cmd.hpp"

namespace leafsync
{
    
class cmd_registry {
public:
    cmd_registry();
    std::string usage() const;

    int run(std::span<const char*> args) const;

    const cmd* find_by_name(const char* name) const;

private:
    std::unordered_map<std::string, const cmd*> _names;
    static const cmd* const _list[];
};

extern const cmd_registry cmds;

}
