#pragma once

#include "cmd.hpp"

namespace leafsync {

class sync_cmd: public cmd {
public:
    sync_cmd();
    virtual int run(const std::span<const char*>& args) const;

private:
    static const char* _names[];

};

}