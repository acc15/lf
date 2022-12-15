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
    if (sz == 0) {
        return result;
    }

    result.append(fmt::format("Usage: \n\n{}", _list[0]->desc));
    for (size_t i = 1; i < std::size(_list); i++) {
        result.append(fmt::format("\n{}", _list[i]->desc));
    }
    return result;
}

const cmd* cmd_registry::find_by_name(const char* name) const {
    const auto it = _names.find(name);
    return it != _names.cend() ? it->second : nullptr;
}

int cmd_registry::run(std::span<const char*> args) const {
    if (args.empty()) {
        fmt::print("{}\n", usage());
        return 1;
    }

    const char* cmd_name = args[0];
    const auto cmd = find_by_name(cmd_name);
    if (cmd == nullptr) {
        fmt::print("Unknown command: {}\n\n{}\n", cmd_name, usage());
        return 1;
    }
    return cmd->run(args.subspan(1));
}


const cmd_registry cmds;

}

