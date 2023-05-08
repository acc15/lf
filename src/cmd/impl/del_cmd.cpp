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
        
        tracked_index& index = ctx.index.get_or_load();
        for (const auto p: ctx.opts[""]) {
            // TODO handle errors
            const fs::path full_path = normalize_path(p);
            const fs::path index_path = relative_path(full_path, ctx.config.local);
            if (ctx.opts.has("force")) {
                fs::remove_all(full_path);
            }
            if (ctx.opts.has("soft")) {
                index.set(index_path, sync_mode::IGNORE);
            } else {
                index.remove(index_path);
            }
        }
        return true;
    }

}