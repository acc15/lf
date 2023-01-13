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
        indexer.process(cfg, opts["shallow"], sync_mode::SHALLOW);
        indexer.process(cfg, opts["recursive"], sync_mode::RECURSIVE);
        indexer.process(cfg, opts["ignore"], sync_mode::IGNORE);
        indexer.save_changes();
        
        return indexer.is_successful() ? 0 : 1;
    }

}