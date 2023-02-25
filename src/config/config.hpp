#pragma once

#include <filesystem>
#include <string>
#include <istream>
#include <concepts>
#include <map>
#include <vector>
#include <span>

#include "io/format.hpp"

namespace lf {

    struct config_format: format<false> {};

    struct config {

        static const char* const name;
        static const char* const env_name;

        struct sync {
            std::string name;
            std::filesystem::path local;
            std::filesystem::path remote;
            std::filesystem::path state = "lf.state";
            std::filesystem::path index = "lf.index";
        };

        using sync_vec = std::vector<sync>;
        using sync_entry_vec = std::vector<const sync*>;
        using sync_entry_match_map = std::map<std::ptrdiff_t, sync_entry_vec>;

        static std::filesystem::path get_path();
        static std::filesystem::path get_default_path();
        
        sync_vec syncs; 

        static config load();

        sync_vec::iterator find_by_name(std::string_view name);
        sync_vec::const_iterator find_by_name(std::string_view name) const;

        sync_entry_vec find_name_matches(const std::vector<std::string_view>& names) const;
        
        sync_entry_match_map find_local_matches(const std::filesystem::path& p) const;
        sync_entry_vec find_most_specific_local_matches(const std::filesystem::path& p) const;

    };

    std::istream& operator>>(std::istream& s, with_format<config_format, config> dest);

}