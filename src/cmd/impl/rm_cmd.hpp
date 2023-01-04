#pragma once

#include "../cmd.hpp"

namespace lf {
    class rm_cmd: public cmd {
    public:
        rm_cmd();
        int run(const std::span<const char*>& args) const override;
    };
}