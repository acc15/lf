#include <fmt/core.h>

#include "cmd_registry.hpp"
#include "sync_cmd.hpp"

namespace leafsync {

const sync_cmd _sync;

const cmd* const cmd_registry::_list[] = { &_sync };

cmd_registry::cmd_registry() {
    for (auto cmd: _list) {
        for (auto name: cmd->desc.names) {
            _names[name] = cmd;
        }
    }
}

std::string cmd_registry::usage() const {
    std::string result;

    const size_t sz = std::size(_list);
    if constexpr (sz == 0) {
        return result;
    }

    result.append(fmt::format("Usage: \n\n{}", _list[0]->desc));
    for (size_t i = 1; i < sz; i++) {
        result.append(fmt::format("\n{}", _list[i]->desc));
    }
    return result;
}

int cmd_registry::run(std::span<const char*> args) const {
    if (args.empty()) {
        fmt::print("{}\n", usage());
        return 0;
    }

    const char* cmd_name = args[0];
    const auto it = _names.find(cmd_name);
    if (it == _names.end()) {
        fmt::print("Unknown command: {}\n\n{}\n", cmd_name, usage());
        return 1;
    }
    return it->second->run(args.subspan(1));
}


const cmd_registry cmds;

}

