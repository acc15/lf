#include "log/config.hpp"

#include <iostream>

#include "log/log.hpp"
#include "log/filter/log_level_filter.hpp"
#include "log/formatter/simple_log_formatter.hpp"
#include "log/sink/stream_log_sink.hpp"
#include "log/sink/rolling_file_sink.hpp"

#include "util/pick.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* log_level_env_name = "LF_LEVEL";
    const char* log_file_env_name = "LF_LOG";

    log_level get_log_level_env() {
        const char* level_env = getenv(log_level_env_name);
        return level_env == nullptr 
            ? log::default_level 
            : parse_enum(level_env, log::default_level, log_level_names);
    }

    fs::path get_log_file_env() {
        const char* path_env = getenv(log_file_env_name);
        return path_env != nullptr ? fs::path(path_env) : get_log_file_default();
    }

    fs::path get_log_file_default() {
        const fs::path rel_path = fs::path("lf") / "lf.log";
#if __linux__
        const char* home = std::getenv("HOME");
        if (home != nullptr) {
            return fs::path(home) / ".cache" / rel_path;
        }
        return fs::path("/var/log") / rel_path;
#elif _WIN32
        const char* local_app_data = std::getenv("LOCALAPPDATA");
        if (local_app_data != nullptr) {
            return fs::path(local_app_data) / rel_path;
        }
        const char* program_data = std::getenv("PROGRAMDATA");
        if (program_data != nullptr) {
            return fs::path(program_data) / rel_path;
        }
        return fs::path("C:\\ProgramData") / rel_path;
#elif __APPLE__
        const char* home = std::getenv("HOME");
        return fs::path(home != nullptr ? home : "/") / "Library" / "Logs" / rel_path;
#else
#       error Unknown platform!
#endif
    }

    void log_init() {
        log.filter = std::make_unique<log_level_filter>(get_log_level_env());
        log.formatter = std::make_unique<simple_log_formatter>();
        log.sinks.clear();
        log.sinks.push_back(std::make_unique<stream_log_sink>(std::cout));
        log.sinks.push_back(std::make_unique<rolling_file_sink>(get_log_file_env(), 100 * 1024 * 1024, 10));
    }

}