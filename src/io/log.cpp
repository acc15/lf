#include <cctype>
#include <cstdlib>

#include <iostream>
#include <iterator>
#include <ostream>
#include <algorithm>
#include <string>

#include "log.hpp"
#include "null_stream.hpp"

namespace lf {

    const char* log::level_names[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "MUTE" };

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

    bool log::with(level level) {
        if (level < min_level) {
            return false;
        }
        next_level = level;
        return true;    
    }

    std::ostream& log::operator()() {
        std::ostream& target = stream_fn(next_level);
        next_level = reset_level;
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

        std::string lower_str = level_str;
        std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](char ch) { return ch >= 0 ? std::toupper(ch) : ch; });
        
        size_t index = std::find(level_names, level_names + std::size(level_names), lower_str) - level_names;
        return std::min(static_cast<level>(index), MUTE);
    }

    class log log;

}