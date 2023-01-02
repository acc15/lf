#pragma once

#include "index/index_tree.hpp"
#include "state/state_tree.hpp"
#include "config/config.hpp"

namespace lf {

    class synchronizer {
    public:

        void sync(config::sync_entry_ptr entry);


    private:

        index_tree _index;
        index_tree _state;

    };

}