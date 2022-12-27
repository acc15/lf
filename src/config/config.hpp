#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <istream>

#include "../io/format.hpp"

namespace lf {

    struct config_sync {
        std::filesystem::path local;
        std::filesystem::path remote;
        std::filesystem::path state;
        std::filesystem::path index;
    };

    using config = std::unordered_map<std::string, config_sync>;
    std::istream& operator>>(std::istream& s, with_format<format::YAML, config&> dest);

    std::filesystem::path get_config_path();

    struct config_desc {
        using type = config;
        static const lf::format format = lf::format::YAML;
        static const char* const name;
    };

}