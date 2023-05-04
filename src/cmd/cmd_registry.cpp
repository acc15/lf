#include "cmd/cmd_registry.hpp"

#include <ostream>
#include <sstream>
#include <iostream>

#include "io/joiner.hpp"
#include "log/log_config.hpp"

#include "cmd/impl/sync_cmd.hpp"
#include "cmd/impl/add_cmd.hpp"
#include "cmd/impl/mv_cmd.hpp"
#include "cmd/impl/del_cmd.hpp"
#include "cmd/impl/info_cmd.hpp"

namespace lf {

    const sync_cmd _sync;
    const add_cmd _add;
    // const mv_cmd _mv;
    const del_cmd _del;
    const info_cmd _info;

    const cmd* const cmd_registry::list[] = { 
        &_sync, 
        &_add, 
        // &_mv,
        &_del,
        &_info
    };

    cmd_registry::cmd_registry() {
        for (const cmd* cmd: list) {
            for (std::string_view name: cmd->names) {
                _names[name] = cmd;
            }
        }
    }

    int cmd_registry::run(std::span<const char*> args) const {
        if (args.empty()) {
            std::cout << *this;
            return 0;
        }

        const char* cmd_name = args[0];
        const auto it = _names.find(cmd_name);
        if (it == _names.end()) {
            std::cerr << "Unknown command: " << cmd_name << std::endl << std::endl << *this;
            return 1;
        }

        const cmd& cmd = *it->second;
        const opt_map opts = cmd.opts.parse(args.subspan(1));
        return cmd.run(opts) ? 0 : 1;
    }

    std::ostream& operator<<(std::ostream& s, const cmd_registry& r) {
        if (std::size(r.list) == 0) {
            return s;
        }

        s << "Usage: " << std::endl << std::endl;
        for (const cmd* cmd: r.list) {
            s << *cmd << std::endl;
        }
        
        s   << "Environment variables: " << std::endl << std::endl
            << config::env_name << " - custom config path, default: " << config::get_default_path().string() << std::endl
            << log_level_env << " - logging level (" << join("|", log_level_names) << "), default: " << log_level_names[log::default_level] << std::endl
            << log_file_env << " - path to log file, default: " << get_log_file_default().string() << std::endl;

        return s;
    }

    const cmd_registry cmds;

}

