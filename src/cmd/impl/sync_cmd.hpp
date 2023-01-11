#pragma once

#include <span>

#include "../cmd.hpp"

namespace lf {

    class sync_cmd: public cmd {
    public:
        sync_cmd();
        int run(const opt_map& opts) const override;
    };

}