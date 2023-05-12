#pragma once

#include "cmd/cmd.hpp"

namespace lf {
    class rm_cmd: public cmd {
    public:
        rm_cmd();
        bool run(cmd_context& ctx) const override;
    };
}