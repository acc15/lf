#include <iostream>
#include <ostream>

#include "log.hpp"
#include "null_stream.hpp"

namespace lf {

    std::ostream& log::default_stream_fn(enum log::level level) {
        return level < min_level ? nullout : level >= ERROR ? std::cerr : std::cout;
    }

    bool log::trace() {
        return with(TRACE);
    }

    bool log::debug() {
        return with(DEBUG);
    }

    bool log::info() {
        return with(INFO);
    }

    bool log::warn() {
        return with(WARN);
    }

    bool log::error() {
        return with(ERROR);
    }

    bool log::with(enum level level) {
        if (level < min_level) {
            return false;
        }
        next_level = level;
        return true;    
    }

    std::ostream& log::operator()() {
        std::ostream& target = stream_fn(next_level);
        next_level = default_level;
        return target;
    }

    class log log;

}