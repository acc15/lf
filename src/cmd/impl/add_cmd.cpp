#include "add_cmd.hpp"
#include "../../io/log.hpp"
#include "../../index/indexer.hpp"

namespace lf {

    add_cmd::add_cmd() : cmd(
        { "a", "add" }, 
        "< [--recursive|-R] <recursive dirs>+ | [--shallow|-S] <file|dir>+ >", 
        "adds specified file and directories to corresponding index files"
    ) {
    }

    int add_cmd::run(const std::span<const char*>& args) const {
        sync_mode mode = sync_mode::SHALLOW;
        
        std::span<const char*> non_opts = args;
        if (!non_opts.empty()) {
            std::string_view opt = args[0];
            if (opt == "--recursive" || opt == "-R") {
                mode = sync_mode::RECURSIVE;
                non_opts = non_opts.subspan(1);
            } else if (opt == "--shallow" || opt == "-S") {
                mode = sync_mode::SHALLOW;
                non_opts = non_opts.subspan(1);
            }
        }

        if (non_opts.empty()) {
            log.error() && log() << *this;
            return 1;
        }

        indexer indexer;
        return indexer.process(non_opts, mode) ? 0 : 1;
    }

}