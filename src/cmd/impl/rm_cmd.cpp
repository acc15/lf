#include <filesystem>

#include "cmd/impl/rm_cmd.hpp"

namespace fs = std::filesystem;

namespace lf {

    rm_cmd::rm_cmd(): cmd(
        { "r", "rm", "remove" }, 
        "removes specified paths from index",
        { 
            { "force", 'F', "also delete file/directory if exists" },
            { "soft", 'S', "soft delete - marks path as IGNORED" },
            { "", 0, "paths to remove", "paths", 0, opt::UNBOUNDED }
        }
    ) {
    }

    bool rm_cmd::run(cmd_context& ctx) const {
        bool ok = true;
        for (const auto p: ctx.opts[""]) {
            const auto pi = normalize_rel(p, ctx.config.left);
            if (!pi) {
                ok = false;
                continue;
            }
            if (ctx.opts.has("force")) {
                fs::remove_all(pi->first);
            }
            if (ctx.opts.has("soft")) {
                ctx.index->set(pi->second, sync_mode::IGNORE);
            } else {
                ctx.index->remove(pi->second);
            }
        }
        return ok;
    }

}