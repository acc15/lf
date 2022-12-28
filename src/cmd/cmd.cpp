#include "cmd/cmd.hpp"

#include "io/joiner.hpp"

namespace lf {

    cmd::cmd(const std::vector<const char*>& names, const char* parameters, const char* description): 
        names(names), 
        parameters(parameters), 
        description(description) 
    {
    }

    std::ostream& operator<<(std::ostream& s, const cmd& c) {
        s << "(" << joiner(c.names, "|") << ")";
        if (c.parameters != nullptr) {
            s << " " << c.parameters;
        }
        if (c.description != nullptr) {
            s << " - " << c.description;
        }
        return s << std::endl;
    }

}