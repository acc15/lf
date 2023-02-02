#pragma once

#include <string>
#include <filesystem>

#include "log/log_level.hpp"

namespace lf {

    extern const char* log_level_env;
    extern const char* log_file_env;

    log_level get_log_level_env();
    std::filesystem::path get_log_file_env();
    std::filesystem::path get_log_file_default();

    void log_init();

}