#include <filesystem>

#include "cmd/impl/rm_cmd.hpp"
#include "index/indexer.hpp"

namespace fs = std::filesystem;

namespace lf {

    rm_cmd::rm_cmd(): cmd(
        { "r", "rm", "d", "del", "delete" }, 
        "removes specified paths from index",
        { 
            { "force", 'F', "also deletes files/directories if exists" },
            { "soft", 'S', "soft delete - marks path as IGNORED" },
            { "", 0, "paths to remove", "paths", 0, opt::UNBOUNDED }
        }
    ) {
    }

    bool rm_cmd::run(const opt_map& params) const {
        indexer indexer;
        for (const auto p: params[""]) {
            const auto path = indexer.resolve(p);
            if (!path.has_value()) {
                continue;
            }
            if (params.has("force")) {
                fs::remove_all(path->abs);
            }
            if (params.has("soft")) {
                indexer.set(path->index, sync_mode::IGNORE);
            } else {
                indexer.remove(path->index);
            }
        }
        return indexer.save_changes();
    }

}