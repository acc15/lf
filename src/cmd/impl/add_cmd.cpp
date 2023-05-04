#include "cmd/impl/add_cmd.hpp"
#include "index/indexer.hpp"

namespace lf {

    add_cmd::add_cmd() : cmd(
        { "a", "add", "set" }, 
        "adds specified file and directories to corresponding index files",
        {
            { "shallow", 'S', "add paths with SHALLOW mode", "paths", 0, opt::UNBOUNDED },
            { "recursive", 'R', "add paths with RECURSIVE mode", "dirpaths", 0, opt::UNBOUNDED },
            { "", '\0', "same as --shallow", "paths", 0, opt::UNBOUNDED },
        },
        "shallow"
    ) {
    }

    bool add_cmd::run(const opt_map& params) const {
        indexer indexer;
        indexer.set_batch(params["shallow"], sync_mode::SHALLOW);
        indexer.set_batch(params["recursive"], sync_mode::RECURSIVE);
        return indexer.save_changes();
    }

}