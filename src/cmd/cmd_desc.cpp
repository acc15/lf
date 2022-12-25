#include "cmd_desc.hpp"
#include "../io/joiner.hpp"

namespace lf {

    std::ostream& operator<<(std::ostream& s, const cmd_desc& desc) {
        s << "(" << joiner(desc.names, "|") << ")";
        if (desc.parameters != nullptr) {
            s << " " << desc.parameters;
        }
        if (desc.description != nullptr) {
            s << " - " << desc.description;
        }
        return s << std::endl;
    }

}