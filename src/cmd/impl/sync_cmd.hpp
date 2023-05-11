#pragma once

#include "cmd/cmd.hpp"

namespace lf {

    class sync_cmd: public cmd {
    public:
        sync_cmd();
        bool run(cmd_context& ctx) const override;
    };

}