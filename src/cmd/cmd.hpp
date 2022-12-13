#pragma once

#include <span>
#include <string>

namespace leafsync {

class cmd {
public:
    cmd(const std::span<const char*>& names, const char* description);
    
    virtual ~cmd() = default;
    virtual int run(const std::span<const char*>& args) const = 0;

    std::span<const char*> names;
    const char* description;
    
    std::string usage() const;

};

}