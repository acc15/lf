#pragma once

#include "cmd/cmd.hpp"

namespace lf {
    class info_cmd: public cmd {
    public:
        info_cmd();
        bool run(cmd_context& ctx) const override;
    };
}