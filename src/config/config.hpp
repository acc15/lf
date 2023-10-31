#pragma once

#include <filesystem>
#include <istream>

#include "io/format.hpp"

namespace lf {

    struct config_format: format<false> {};

    struct config {

        static const char* const name;
        static const char* const env_name;

        static std::filesystem::path get_path();
        static std::filesystem::path get_default_path();
        
        std::filesystem::path local;
        std::filesystem::path remote;
        std::filesystem::path state = "lf.state";
        std::filesystem::path index = "lf.index";

        static config parse(std::istream& stream);
        static config load();

    };

}