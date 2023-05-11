#include "cmd/impl/sync_cmd.hpp"
#include "sync/synchronizer.hpp"

namespace lf {

    sync_cmd::sync_cmd(): cmd(
        { "s", "sync" }, 
        "synchronizes mirrors", 
        {}
    ) {}

    bool sync_cmd::run(cmd_context& ctx) const {
        synchronizer s(ctx.config, *ctx.index, *ctx.state);
        s.run();
        return true;
    }

}