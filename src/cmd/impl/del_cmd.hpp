#pragma once

#include "../cmd.hpp"

namespace lf {
    class del_cmd: public cmd {
    public:
        del_cmd();
        bool run(const opt_map& params) const override;
    };
}