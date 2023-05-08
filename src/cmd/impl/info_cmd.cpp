#include "cmd/impl/info_cmd.hpp"

#include "config/config.hpp"
#include "state/state.hpp"
#include "index/index.hpp"

namespace lf {

    info_cmd::info_cmd(): cmd(
        {"i", "info"}, 
        "prints index",
        { 
            { "state", 'S', "prints state"} 
        }
    ) {
    }
    
    bool info_cmd::run(cmd_context& ctx) const {
        if (ctx.opts.has("state")) {
            std::cout << ctx.state.get_or_load().root;
        } else {
            std::cout << ctx.index.get_or_load().root;
        }
        return true;
    }

}