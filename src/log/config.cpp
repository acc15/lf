#include "log/config.hpp"

#include <iostream>

#include "log/log.hpp"
#include "log/filter/log_level_filter.hpp"
#include "log/formatter/simple_log_formatter.hpp"
#include "log/sink/stream_log_sink.hpp"

namespace lf {

    const log_level log_level_default = log_level::INFO;
    const char* log_level_env_name = "LF_LEVEL";
    const char* log_file_env_name = "LF_LOG";

    log_level get_log_level_env() {
        const char* level_env = getenv(log_level_env_name);
        if (level_env == nullptr) {
            return log_level_default;
        }
        // TODO parse log level
        return INFO;
    }

    std::filesystem::path get_log_file_env() {
        const char* path_env = getenv(log_file_env_name);
        return path_env != nullptr ? std::filesystem::path(path_env) : get_log_file_default();
    }

    std::filesystem::path get_log_file_default() {
        return std::filesystem::path();
    }

    void log_init() {
        log.filter = std::make_unique<log_level_filter>(get_log_level_env());
        log.formatter = std::make_unique<simple_log_formatter>();
        log.sinks.clear();
        log.sinks.push_back(std::make_unique<stream_log_sink>(std::cout));
    }

}