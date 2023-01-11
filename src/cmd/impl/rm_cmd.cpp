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

    int rm_cmd::run(const opt_map& opts) const {
        const config cfg = config::load();

        indexer indexer;
        bool success = indexer.process(cfg, opts[""], sync_mode::UNSPECIFIED);
        success &= indexer.save_changes();
        return success ? 0 : 1;
    }

}