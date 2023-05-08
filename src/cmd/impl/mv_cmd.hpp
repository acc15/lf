#pragma once

#include "cmd/cmd.hpp"

namespace lf {

    class mv_cmd: public cmd {
    public:
        explicit mv_cmd();
        bool run(cmd_context& ctx) const override;
    };

}