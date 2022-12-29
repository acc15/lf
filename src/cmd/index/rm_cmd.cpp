#include "rm_cmd.hpp"
#include "../../index/indexer.hpp"
#include "../../io/log.hpp"

namespace lf {

    rm_cmd::rm_cmd(): cmd({ "r", "rm" }, "< file | dir >+", "removes specified files and directories from corresponding index files") {
    }

    int rm_cmd::run(const std::span<const char*>& args) const {
        if (args.empty()) {
            log.error() && log() << *this;
            return 1;
        }

        indexer indexer;
        return indexer.process(args, sync_mode::NONE) ? 0 : 1;
    }

}