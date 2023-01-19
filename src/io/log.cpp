#include <cstdlib>

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>

#include "util/string.hpp"
#include "io/log.hpp"
#include "io/null_stream.hpp"

namespace lf {

    const char* const log::env_name = "LF_LEVEL";
    const char* const log::level_names[6] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "MUTE" };

    std::ostream& log::default_stream(log::level level) {
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

    bool log::with(level level) {
        if (level < min_level) {
            return false;
        }
        cur_level = level;
        return true;    
    }

    std::ostream& log::operator()() {
        std::ostream& target = stream != nullptr ? *stream : default_stream(cur_level);
        cur_level = default_level;
        return target;
    }

    log::level log::get_default_min_level() {
        const char* level_str = std::getenv("LF_LEVEL");
        if (level_str == nullptr) {
            return default_level;
        }
        
        int v = std::atoi(level_str);
        if (v > 0) {
            return std::min(static_cast<level>(v - 1), MUTE);
        }

        std::string upper_str = level_str;
        upper(upper_str);
        
        const auto it = std::find(std::begin(level_names), std::end(level_names), upper_str);
        return it == std::end(level_names) 
            ? default_level 
            : static_cast<level>(it - std::begin(level_names));
    }

    struct log log;

}