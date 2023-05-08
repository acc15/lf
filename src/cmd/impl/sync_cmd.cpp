#include "io/adv_fstream.hpp"
#include "fs/serialization.hpp"
#include "cmd/impl/sync_cmd.hpp"
#include "config/config.hpp"
#include "log/log.hpp"
#include "io/joiner.hpp"

#include "sync/synchronizer.hpp"

namespace lf {

    sync_cmd::sync_cmd(): cmd(
        { "s", "sync" }, 
        "synchronizes mirrors", 
        {}
    ) {}

    bool sync_cmd::run(cmd_context& ctx) const {
        synchronizer(ctx.config, ctx.index.get_or_load(), ctx.state.get_or_load()).run();
        return true;
    }

}