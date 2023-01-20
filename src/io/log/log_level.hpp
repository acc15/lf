#pragma once

#include <ostream>

namespace lf {

    enum log_level { 
        TRACE, 
        DEBUG, 
        INFO, 
        WARN, 
        ERROR, 
        MUTE 
    };

    extern const char* log_level_names[MUTE + 1];

    std::ostream& operator<<(std::ostream& s, const log_level& l);

}