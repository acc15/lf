#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <istream>
#include <concepts>
#include <map>
#include <vector>

#include "io/format.hpp"

namespace lf {

    struct config_sync {
        std::filesystem::path local;
        std::filesystem::path remote;
        std::filesystem::path state;
        std::filesystem::path index;
    };

    struct config {

        using type = config;
        static const lf::format format = lf::format::TEXT;
        static const char* const name;

        using sync_map = std::unordered_map<std::string, config_sync>;
        using match_pair_ptr = sync_map::const_pointer;
        using match_vec = std::vector<match_pair_ptr>;
        using match_map = std::map<std::ptrdiff_t, match_vec>;

        static std::filesystem::path get_path();
        
        sync_map syncs; 

        static config load();
        match_map find_matches(const std::filesystem::path& p) const;
        match_vec find_most_specific_matches(const std::filesystem::path& p) const;

    };

    std::istream& operator>>(std::istream& s, with_format<format::TEXT, config&> dest);

}