#pragma once

#include <concepts>
#include <filesystem>

#include "../fs/path.hpp"
#include "../io/log.hpp"
#include "config.hpp"

namespace lf {

    template<std::invocable<const std::string&, const config_sync&> Op> 
    bool find_syncs_by_path(const config& c, const std::filesystem::path& p, Op op) {
        bool sync_found = false;
        for (const auto& e: c) {
            if (!is_subpath(p, e.second.local)) {
                log.trace() && log() 
                    << "sync \"" << e.first << "\" skipped due to base path mismatch (" 
                    << e.second.local << " and " << p << ")" 
                    << std::endl;
                continue;
            }
            op(e.first, e.second);
            sync_found = true;
        }
        if (!sync_found) {
            log.error() && log() << "no configured syncs found for path " << p << std::endl;
        }
        return sync_found;
    }

}