#include "cmd/impl/info_cmd.hpp"

#include "config/config.hpp"
#include "state/state.hpp"
#include "index/index.hpp"

namespace lf {

    info_cmd::info_cmd(): cmd(
        {"i", "info"}, 
        "prints index or state tree",
        { 
            { "state", 'S', "prints state"} 
        }
    ) {
    }
    
    bool info_cmd::run(const opt_map& params) const {
        const config cfg = config::load();
        if (params.has("state")) {
            print_tree<state>(cfg.state);
        } else {
            print_tree<index>(cfg.index);
        }
        return true;
    }

}