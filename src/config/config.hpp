#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <istream>
#include <concepts>

#include "io/format.hpp"
#include "io/log.hpp"
#include "fs/path.hpp"

namespace lf {

    struct config_sync {
        std::filesystem::path local;
        std::filesystem::path remote;
        std::filesystem::path state;
        std::filesystem::path index;
    };

    using config = std::unordered_map<std::string, config_sync>;

    struct config_desc {
        using type = config;
        static const lf::format format = lf::format::TEXT;
        static const char* const name;
    };

    std::istream& operator>>(std::istream& s, with_format<format::TEXT, config&> dest);

    std::filesystem::path get_config_path();

    template<std::invocable<const config::value_type&> Op> 
    bool find_config_sync_by_path(const config& c, const std::filesystem::path& p, Op op) {
        bool sync_found = false;
        for (const auto& e: c) {
            if (!is_subpath(p, e.second.local)) {
                log.trace() && log() 
                    << "sync \"" << e.first << "\" skipped due to base path mismatch (" 
                    << e.second.local << " and " << p << ")" 
                    << std::endl;
                continue;
            }
            op(e);
            sync_found = true;
        }
        if (!sync_found) {
            log.error() && log() << "no configured syncs found for path " << p << std::endl;
        }
        return sync_found;
    }

}