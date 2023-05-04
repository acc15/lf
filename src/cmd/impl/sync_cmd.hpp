#pragma once

#include <span>

#include "config/config.hpp"
#include "../cmd.hpp"

namespace lf {

    class sync_cmd: public cmd {
        bool do_sync(const config& config) const;
        void do_sync_safe(const config& config) const;
    
    public:
        sync_cmd();
        bool run(const opt_map& params) const override;
    };

}