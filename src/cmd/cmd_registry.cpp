#include <ostream>
#include <sstream>
#include <iostream>

#include "cmd_registry.hpp"
#include "sync_cmd.hpp"
#include "add_cmd.hpp"
#include "rm_cmd.hpp"

namespace lf {

const sync_cmd _sync;
const add_cmd _add;
const rm_cmd _rm;

const cmd* const cmd_registry::_list[] = { 
    &_sync, 
    &_add, 
    &_rm 
};

cmd_registry::cmd_registry() {
    for (auto cmd: _list) {
        for (auto name: cmd->desc.names) {
            _names[name] = cmd;
        }
    }
}

std::ostream& cmd_registry::usage(std::ostream& s) const {
    const size_t sz = std::size(_list);
    if constexpr (sz == 0) {
        return s;
    }

    s << "Usage: " << std::endl << std::endl;
    for (const auto& cmd: _list) {
        s << cmd->desc;
    }
    return s;
}

int cmd_registry::run(std::span<const char*> args) const {
    if (args.empty()) {
        usage(std::cout);
        return 0;
    }

    const char* cmd_name = args[0];
    const auto it = _names.find(cmd_name);
    if (it == _names.end()) {
        std::cerr << "Unknown command: " << cmd_name << std::endl << std::endl;
        usage(std::cerr);
        return 1;
    }
    return it->second->run(args.subspan(1));
}

const cmd_registry cmds;

}

