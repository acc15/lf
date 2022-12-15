#pragma once

#include <span>

#include "cmd_desc.hpp"

namespace leafsync {

class cmd {
public:
    cmd(const cmd_desc& desc);
    
    virtual ~cmd() = default;
    virtual int run(const std::span<const char*>& args) const = 0;

    const cmd_desc desc;

};

}