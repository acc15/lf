#include <filesystem>

#include "cmd/impl/del_cmd.hpp"

namespace fs = std::filesystem;

namespace lf {

    del_cmd::del_cmd(): cmd(
        { "d", "del", "delete" }, 
        "deletes specified paths from index",
        { 
            { "force", 'F', "also deletes files/directories if exists" },
            { "soft", 'S', "soft delete - marks path as IGNORED" },
            { "", 0, "paths to remove", "paths", 0, opt::UNBOUNDED }
        }
    ) {
    }

    bool del_cmd::run(cmd_context& ctx) const {
        
        bool ok = true;
        for (const auto p: ctx.opts[""]) {
            const auto pi = make_rel_path_info(p, ctx.config.local);
            if (!pi) {
                ok = false;
                continue;
            }
            if (ctx.opts.has("force")) {
                fs::remove_all(pi->abs);
            }
            if (ctx.opts.has("soft")) {
                ctx.index->set(pi->rel, sync_mode::IGNORE);
            } else {
                ctx.index->remove(pi->rel);
            }
        }
        return ok;
    }

}