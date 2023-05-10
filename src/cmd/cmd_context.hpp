#pragma once

#include "opts/opt_map.hpp"
#include "config/config.hpp"
#include "index/index.hpp"
#include "state/state.hpp"
#include "tree/lazy_tree.hpp"

namespace lf {

    struct cmd_context {
        const opt_map opts;
        const lf::config config;
        lazy_tree<lf::index> index;
        lazy_tree<lf::state> state;
    };

}