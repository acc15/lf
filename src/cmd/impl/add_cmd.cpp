#include "cmd/impl/add_cmd.hpp"
#include "index/indexer.hpp"

namespace lf {

    add_cmd::add_cmd() : cmd(
        { "a", "add", "set" }, 
        "adds specified file and directories to corresponding index files",
        {
            opt { "shallow", 'S', "add paths with SHALLOW mode", "paths", 0, opt::UNBOUNDED },
            opt { "recursive", 'R', "add paths with RECURSIVE mode", "dirpaths", 0, opt::UNBOUNDED },
            opt { "ignore", 'I', "add paths with IGNORE mode", "paths", 0, opt::UNBOUNDED },
            opt { "", '\0', "same as --shallow", "paths", 0, opt::UNBOUNDED },
        },
        "shallow"
    ) {
    }

    bool add_cmd::run(const opt_map& params) const {
        indexer indexer;
        indexer.init();
        indexer.process(params["shallow"], sync_mode::SHALLOW);
        indexer.process(params["recursive"], sync_mode::RECURSIVE);
        indexer.process(params["ignore"], sync_mode::IGNORE);
        return indexer.save_changes();
    }

}