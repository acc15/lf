#include "log/log_config.hpp"

#include <iostream>

#include "log/log.hpp"
#include "log/filter/log_level_filter.hpp"
#include "log/formatter/simple_log_formatter.hpp"
#include "log/sink/stream_log_sink.hpp"
#include "log/sink/rolling_file_sink.hpp"

#include "util/pick.hpp"
#include "fs/os_path.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* log_level_env = "LF_LEVEL";
    const char* log_file_env = "LF_LOG";

    log_level get_log_level_env() {
        const char* level_env = getenv(log_level_env);
        return level_env == nullptr 
            ? log::default_level 
            : parse_enum(level_env, log::default_level, log_level_names);
    } 

    fs::path get_log_file_env() {
        const char* path_env = getenv(log_file_env);
        return path_env != nullptr ? fs::path(path_env) : get_log_file_default();
    }

    fs::path get_log_file_default() {
        return get_os_base_path(os_path_kind::LOG) / "lf" / "lf.log";
    }

    void log_init() {
        log.filter = std::make_unique<log_level_filter>(get_log_level_env());
        log.formatter = std::make_unique<simple_log_formatter>();
        log.sinks.clear();
        log.sinks.push_back(std::make_unique<stream_log_sink>(std::cout));
        log.sinks.push_back(std::make_unique<rolling_file_sink>(get_log_file_env(), 10 * 1024 * 1024, 9));
    }

}