#include <sstream>
#include <iostream>

#include "cmd_registry.hpp"
#include "sync_cmd.hpp"

namespace lf {

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
    std::stringstream s;

    const size_t sz = std::size(_list);
    if constexpr (sz == 0) {
        return s.str();
    }

    s << "Usage: " << std::endl << std::endl << _list[0]->desc;
    for (size_t i = 1; i < sz; i++) {
        s << std::endl << _list[i]->desc;
    }
    return s.str();
}

int cmd_registry::run(std::span<const char*> args) const {
    if (args.empty()) {
        std::cout << usage() << std::endl;
        return 0;
    }

    const char* cmd_name = args[0];
    const auto it = _names.find(cmd_name);
    if (it == _names.end()) {
        std::cout << "Unknown command: " << cmd_name << std::endl << std::endl << usage() << std::endl;
        return 1;
    }
    return it->second->run(args.subspan(1));
}


const cmd_registry cmds;

}

