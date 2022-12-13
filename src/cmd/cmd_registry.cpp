#include <fmt/ranges.h>

#include "cmd_registry.hpp"
#include "sync_cmd.hpp"

namespace leafsync {

const sync_cmd _sync;
const cmd* _cmds[] = { &_sync };

cmd_registry::cmd_registry([[maybe_unused]] const std::span<const cmd*>& cmds): cmds(cmds) {
}

std::string cmd_registry::usage() const {
    std::string str = "";
    for (auto cmd: cmds) {
        str += cmd->usage() + "\n";
    }
    return str;
}

const cmd_registry cmds(_cmds);

}

