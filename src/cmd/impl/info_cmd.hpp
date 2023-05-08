#pragma once

#include "cmd/cmd.hpp"
#include "fs/serialization.hpp"
#include "tree/print.hpp"
#include "tree/binary.hpp"

#include <iostream>

namespace lf {
    class info_cmd: public cmd {
    public:
        info_cmd();
        bool run(cmd_context& ctx) const override;
    };
}