#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <istream>
#include <concepts>
#include <map>
#include <vector>
#include <span>

#include "io/format.hpp"

namespace lf {

    struct config_sync {
        std::filesystem::path local;
        std::filesystem::path remote;
        std::filesystem::path state;
        std::filesystem::path index;
    };

    struct config {

        static const lf::format format = lf::format::TEXT;
        static const char* const name;
        static const char* const env_name;

        using sync_map = std::unordered_map<std::string, config_sync>;
        using sync_entry_ptr = sync_map::const_pointer;
        using sync_entry_vec = std::vector<sync_entry_ptr>;
        using sync_entry_match_map = std::map<std::ptrdiff_t, sync_entry_vec>;

        static std::filesystem::path get_path();
        static std::filesystem::path get_default_path();
        
        sync_map syncs; 

        static config load();

        sync_entry_vec find_name_matches(const std::span<const char*>& names) const;
        
        sync_entry_match_map find_local_matches(const std::filesystem::path& p) const;
        sync_entry_vec find_most_specific_local_matches(const std::filesystem::path& p) const;

    };

    std::istream& operator>>(std::istream& s, with_format<format::TEXT, config&> dest);

}