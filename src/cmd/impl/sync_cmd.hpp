#pragma once

#include <span>

#include "config/config.hpp"
#include "../cmd.hpp"

namespace lf {

    class sync_cmd: public cmd {
    public:
        sync_cmd();
        int run(const opt_map& params) const override;

    private:
        bool do_sync(const std::string& name, const config::sync& sync) const;
    };

}