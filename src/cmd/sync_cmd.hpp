#pragma once

#include "cmd.hpp"

namespace leafsync {

class sync_cmd: public cmd {
public:
    sync_cmd();
    int run(const std::span<const char*>& args) const override;
};

}