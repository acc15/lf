#pragma once

#include "index/index.hpp"
#include "state/state.hpp"
#include "config/config.hpp"
#include "sync/sync_item.hpp"

#include <vector>

namespace lf {
        
    class synchronizer {
    public:
        synchronizer(const config::sync& sync);

        void run();
        void load();
        void save();

        tracked_index index;
        tracked_state state;
        const lf::config::sync& config;
        std::vector<sync_item> queue;

    };

}