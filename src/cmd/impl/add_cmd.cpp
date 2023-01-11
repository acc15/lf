#include "cmd/impl/add_cmd.hpp"
#include "index/indexer.hpp"

namespace lf {

    add_cmd::add_cmd() : cmd(
        { "a", "add" }, 
        "adds specified file and directories to corresponding index files",
        {
            opt { "shallow", 'S', "add paths with SHALLOW mode", "paths", 0, opt::UNBOUNDED },
            opt { "recursive", 'R', "add paths with RECURSIVE mode", "dirpaths", 0, opt::UNBOUNDED },
            opt { "ignore", 'I', "add paths with IGNORE mode", "paths", 0, opt::UNBOUNDED },
            opt { "", '\0', "same as with --shallow", "paths", 0, opt::UNBOUNDED },
        },
        "shallow"
    ) {
    }

    int add_cmd::run(const opt_map& opts) const {

        config cfg = config::load();

        indexer indexer;
        
        bool success = true;
        success &= indexer.process(cfg, opts["shallow"], sync_mode::SHALLOW);
        success &= indexer.process(cfg, opts["recursive"], sync_mode::RECURSIVE);
        success &= indexer.process(cfg, opts["ignore"], sync_mode::IGNORE);
        success &= indexer.save_changes();
        return success ? 0 : 1;
    }

}