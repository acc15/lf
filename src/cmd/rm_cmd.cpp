#include "rm_cmd.hpp"
#include "../index/indexer.hpp"
#include "../io/log.hpp"

namespace lf {

    const char* rm_cmd::names[] = { "r", "rm" };
    const cmd_desc rm_cmd::desc = {
        names, 
        "<file|dir>+", 
        "removes specified files and directories from corresponding index files"
    };

    rm_cmd::rm_cmd(): cmd(desc) {
    }

    int rm_cmd::run(const std::span<const char*>& args) const {
        if (args.empty()) {
            log.error() && log() << desc;
            return 1;
        }

        indexer indexer;
        return indexer.process(args, sync_mode::NONE) ? 0 : 1;
    }

}