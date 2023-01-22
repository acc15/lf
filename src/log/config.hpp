#pragma once

#include <string>
#include <filesystem>

#include "log/log_level.hpp"

namespace lf {

    extern const log_level log_level_default;
    extern const char* log_level_env_name;
    extern const char* log_file_env_name;

    log_level get_log_level_env();
    std::filesystem::path get_log_file_env();
    std::filesystem::path get_log_file_default();

    void log_init();

}