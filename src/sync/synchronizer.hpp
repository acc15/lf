#pragma once

#include "index/index_tree.hpp"
#include "state/state_tree.hpp"
#include "config/config.hpp"

namespace lf {

    class synchronizer {
    public:

        synchronizer(const config::sync_entry& sync);

        void run();

    private:

        bool init();

        const config::sync_entry& sync;
        index_tree index;
        state_tree state;

    };

}