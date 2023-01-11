#include "cmd/cmd.hpp"

#include "io/joiner.hpp"

namespace lf {

    cmd::cmd(
        const std::initializer_list<std::string_view>& names, 
        std::string_view description, 
        const std::initializer_list<opt>& opts, 
        std::string_view default_opt
    ): 
        names(names), 
        description(description),
        opts { opts, default_opt }
    {
    }

    std::ostream& operator<<(std::ostream& s, const cmd& c) {
        s << join(", ", c.names);
        if (!c.description.empty()) {
            s << " - " << c.description;
        }
        return s << std::endl << c.opts;
    }

}