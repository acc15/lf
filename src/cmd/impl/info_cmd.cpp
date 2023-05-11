#include "cmd/impl/info_cmd.hpp"

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
            std::cout << ctx.state->root;
        } else {
            std::cout << ctx.index->root;
        }
        return true;
    }

}