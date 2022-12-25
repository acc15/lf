#pragma once

#include <span>
#include "cmd.hpp"
#include "cmd_desc.hpp"

namespace lf {

class sync_cmd: public cmd {
public:

    static const char* names[];
    static const cmd_desc desc;

    sync_cmd();
    int run(const std::span<const char*>& args) const override;
};

}