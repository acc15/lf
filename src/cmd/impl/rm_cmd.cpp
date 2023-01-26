#include "cmd/impl/rm_cmd.hpp"
#include "index/indexer.hpp"

namespace lf {

    rm_cmd::rm_cmd(): cmd(
        { "r", "rm" }, 
        "removes specified files and directories from corresponding index files",
        { { "", 0, "paths to remove from index", "paths", 0, opt::UNBOUNDED } }
    ) {
    }

    bool rm_cmd::run(const opt_map& params) const {
        const config cfg = config::load();

        indexer indexer;
        indexer.process(cfg, params[""], std::nullopt);
        indexer.save_changes();

        return indexer.is_successful();
    }

}