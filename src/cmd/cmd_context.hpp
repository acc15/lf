#pragma once

#include "opts/opt_map.hpp"
#include "config/config.hpp"
#include "index/index.hpp"
#include "state/state.hpp"
#include "tree/tree_loader.hpp"

namespace lf {

    struct cmd_context {
        const opt_map opts;
        const lf::config config;
        tree_loader<lf::index> index;
        tree_loader<lf::state> state;
    };

}