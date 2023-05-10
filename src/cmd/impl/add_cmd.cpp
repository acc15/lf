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
        return add_paths_to_index(ctx, "shallow", sync_mode::SHALLOW) 
            && add_paths_to_index(ctx, "recursive", sync_mode::RECURSIVE);
    }

    bool add_cmd::add_paths_to_index(cmd_context& ctx, const char* opt_name, sync_mode mode) {
        bool ok = true;
        for (const auto p: ctx.opts[opt_name]) {
            const auto pi = make_rel_path_info(p, ctx.config.local);
            if (pi) {
                ctx.index->set(pi->rel, mode);
            } else {
                ok = false;
            }
        }
        return ok;
    }

}