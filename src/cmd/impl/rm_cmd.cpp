#include "cmd/impl/rm_cmd.hpp"
#include "index/indexer.hpp"

namespace lf {

    rm_cmd::rm_cmd(): cmd(
        { "r", "rm", "d", "del", "delete" }, 
        "removes specified files and directories from index files",
        { 
            { "force", 'F', "also deletes file/directory" },
            { "", 0, "paths to remove", "paths", 0, opt::UNBOUNDED }
        }
    ) {
    }

    bool rm_cmd::run(const opt_map& params) const {
        indexer indexer;
        indexer.process(params[""], params.has("force") ? std::nullopt : std::optional<sync_mode>(sync_mode::IGNORE));
        return indexer.save_changes();
    }

}