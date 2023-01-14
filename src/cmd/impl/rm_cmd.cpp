#include "cmd/impl/rm_cmd.hpp"
#include "index/indexer.hpp"
#include "io/log.hpp"
#include "util/string.hpp"

namespace lf {

    rm_cmd::rm_cmd(): cmd(
        { "r", "rm" }, 
        "removes specified files and directories from corresponding index files",
        { { "", 0, "paths to remove from index", "paths", 0, opt::UNBOUNDED } }
    ) {
    }

    int rm_cmd::run(const opt_map& params) const {
        const config cfg = config::load();

        indexer indexer;
        indexer.process(cfg, params[""], sync_mode::UNSPECIFIED);
        indexer.save_changes();

        return indexer.is_successful() ? 0 : 1;
    }

}