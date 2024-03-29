#pragma once

#include <vector>
#include <string_view>
#include <ostream>

#include "opts/opts.hpp"
#include "cmd/cmd_context.hpp"

namespace lf {

    class cmd {
    public:
        cmd(
            const std::initializer_list<std::string_view>& names, 
            std::string_view description, 
            const std::initializer_list<opt>& opts, 
            std::string_view default_opt = ""
        );

        virtual ~cmd() = default;
        virtual bool run(cmd_context& ctx) const = 0;

        std::vector<std::string_view> names;
        std::string_view description;
        struct opts opts;
    };

    std::ostream& operator<<(std::ostream& s, const cmd& c);


}