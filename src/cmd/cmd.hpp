#pragma once

#include <span>
#include <vector>
#include <string_view>
#include <ostream>

namespace lf {

    class cmd {
    public:
        cmd(const std::vector<const char*>& names, const char* parameters, const char* description);
        virtual ~cmd() = default;
        virtual int run(const std::span<const char*>& args) const = 0;

        std::vector<const char*> names;
        const char* parameters;
        const char* description;
    };

    std::ostream& operator<<(std::ostream& s, const cmd& c);


}