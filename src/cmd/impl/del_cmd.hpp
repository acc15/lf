#pragma once

#include "cmd/cmd.hpp"

namespace lf {
    class del_cmd: public cmd {
    public:
        del_cmd();
        bool run(cmd_context& ctx) const override;
    };
}