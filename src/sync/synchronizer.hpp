#pragma once

#include "index/index.hpp"
#include "state/state.hpp"
#include "config/config.hpp"
#include "sync/sync_item.hpp"

#include <vector>

namespace lf {
        
    class synchronizer {
    public:
        synchronizer(const config& config, const lf::index& index, tracked_state& state);

        void run();

        const lf::config& config;
        const lf::index& index;
        tracked_state& state;
 
        std::vector<sync_item> queue;

    };

}