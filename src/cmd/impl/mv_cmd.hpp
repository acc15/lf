#pragma once

#include "cmd/cmd.hpp"

namespace lf {

    class mv_cmd: public cmd {
    public:
        explicit mv_cmd();
        bool run(const opt_map& params) const override;
    };

}