#include "cmd_desc.hpp"
#include "../io/joiner.hpp"

namespace lf {

    std::ostream& operator<<(std::ostream& s, const cmd_desc& desc) {
        return s << joiner(desc.names, ", ") << " - " << desc.description;
    }

}