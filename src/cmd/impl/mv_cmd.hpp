#pragma once

#include "cmd/cmd.hpp"

namespace lf {

    class mv_cmd: public cmd {
    public:
        explicit mv_cmd();
        bool run(cmd_context& ctx) const override;
        bool move(cmd_context& ctx, std::string_view from, std::string_view to, bool move_file) const;
    };

}