#pragma once

#include "cmd.hpp"

namespace lf {
    class rm_cmd: public cmd {
    public:
        static const char* names[];
        static const cmd_desc desc;

        rm_cmd();
        int run(const std::span<const char*>& args) const override;
    };
}