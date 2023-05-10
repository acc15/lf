#include "cmd/impl/add_cmd.hpp"

namespace lf {

    add_cmd::add_cmd() : cmd(
        { "a", "add", "set" }, 
        "adds specified file and directories to corresponding index files",
        {
            { "shallow", 'S', "add paths with SHALLOW mode", "paths", 0, opt::UNBOUNDED },
            { "recursive", 'R', "add paths with RECURSIVE mode", "dirpaths", 0, opt::UNBOUNDED },
            { "", '\0', "same as --shallow", "paths", 0, opt::UNBOUNDED },
        },
        "shallow"
    ) {
    }

    bool add_cmd::run(cmd_context& ctx) const {
        tracked_index& index = ctx.index.get_or_load();
        // TODO handle errors
        for (const auto shallow_path: ctx.opts["shallow"]) {
            index.set(relative_path(absolute_path(shallow_path), ctx.config.local).value(), sync_mode::SHALLOW);
        }
        for (const auto recursive_path: ctx.opts["recursive"]) {
            index.set(relative_path(absolute_path(recursive_path), ctx.config.local).value(), sync_mode::RECURSIVE);
        }
        return true;
    }

}