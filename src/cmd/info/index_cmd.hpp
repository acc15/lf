#pragma once

#include "cmd/cmd.hpp"

namespace lf {
    class index_cmd: public cmd {
    public:
        index_cmd();
        int run(const std::span<const char*>& args) const override;
    };
}