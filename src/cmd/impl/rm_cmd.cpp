#include "cmd/impl/rm_cmd.hpp"
#include "index/indexer.hpp"
#include "io/log.hpp"
#include "util/string.hpp"

namespace lf {

    rm_cmd::rm_cmd(): cmd(
        { "r", "rm" }, 
        "([--unspecify|-u] (file|dir)+ | (--ignore|-i) (file|dir)+)", 
        "removes specified files and directories from corresponding index files"
    ) {
    }

    int rm_cmd::run(const std::span<const char*>& args) const {
        if (args.empty()) {
            log.error() && log() << *this;
            return 1;
        }

        sync_mode mode = sync_mode::UNSPECIFIED;
        std::span<const char*> non_opts = args;
        if (!non_opts.empty()) {
            std::string opt = lower(non_opts[0]);
            if (opt == "--ignore" || opt == "-i") {
                mode = sync_mode::IGNORE;
                non_opts = non_opts.subspan(1);
            } else if (opt == "--unspecify" || opt == "-u") {
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