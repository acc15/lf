#pragma once

#include "cmd/cmd.hpp"

namespace lf {

    class mv_cmd: public cmd {
    public:
        explicit mv_cmd();
        bool run(cmd_context& ctx) const override;
        bool move(cmd_context& ctx, const path_pair& from, const path_pair& to) const;
    };

}