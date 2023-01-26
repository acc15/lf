#pragma once

#include "../cmd.hpp"

namespace lf {
    class rm_cmd: public cmd {
    public:
        rm_cmd();
        bool run(const opt_map& params) const override;
    };
}