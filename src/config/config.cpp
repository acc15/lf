#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>

#include "io/serialization.hpp"

#include "config/config.hpp"
#include "config/config_parser.hpp"

namespace fs = std::filesystem;

namespace lf {

    bool check_config_path_specified(const std::string& name, const std::filesystem::path& p, const char* key) {
        if (!p.empty()) {
            return true;
        }
        log.error() && log() << "sync \"" << name << "\" " << key << " path isn't specified" << std::endl;
        return false;
    }

    std::istream& operator>>(std::istream& s, with_format<format::TEXT, config&> dest) {
        config& cfg = dest.value;
        cfg.syncs.clear();
        
        parse_config(s, [&cfg](const config_entry& e) {
            if (e.section.empty()) {
                log.error() && log() << "empty section at line " << e.line << std::endl;
                return;
            }
            if (e.value.empty()) {
                log.error() && log() << "empty value for key " << e.key << " at line " << e.line << std::endl;
                return;
            }

            config_sync& sync = cfg.syncs[e.section];
            if (e.key == "local") {
                sync.local = e.value;
                if (!sync.local.is_absolute()) {
                    log.error() && log() << "local path must be absolute at line " << e.line << ": " << e.value << std::endl;
                }
            } else if (e.key == "remote") {
                sync.remote = e.value;
                if (!sync.remote.is_absolute()) {
                    log.error() && log() << "remote path must be absolute at line " << e.line << ": " << e.value << std::endl;
                }
            } else if (e.key == "index") {
                sync.index = e.value;
            } else if (e.key == "state") {
                sync.state = e.value;
            } else {
                log.error() && log() << "invalid key " << e.key << " at line " << e.line << std::endl;
            }
        });

        auto it = cfg.syncs.begin();
        while (it != cfg.syncs.end()) {
            config_sync& sync = it->second;
            
            bool valid = check_config_path_specified(it->first, sync.local, "local");
            valid &= check_config_path_specified(it->first, sync.remote, "remote");
            valid &= check_config_path_specified(it->first, sync.index, "index");
            valid &= check_config_path_specified(it->first, sync.state, "state");

            if (valid) {
                if (sync.index.is_relative()) {
                    sync.index = sync.remote / sync.index;
                }
                if (sync.state.is_relative()) {
                    sync.state = sync.local / sync.state;
                }
                ++it;
            } else {
                s.setstate(std::ios_base::failbit);
                it = cfg.syncs.erase(it);
            }
        }

        if (cfg.syncs.empty()) {
            log.error() && log() << "config doesn't have any valid sync entry" << std::endl;
            s.setstate(std::istream::failbit);
        }
        return s;
    }
    
    fs::path config::get_path() {
        const char* config_path = std::getenv("LF_CONFIG");
        if (config_path != nullptr) {
            return config_path;
        }

        const fs::path rel_path = "lf/lf.conf";

#if __linux__
        const char* home = std::getenv("HOME");
        if (home != nullptr) {
            return fs::path(home) / ".config" / rel_path;
        }
        return fs::path("/etc") / rel_path;
#elif _WIN32
        const char* local_app_data = std::getenv("LOCALAPPDATA");
        if (local_app_data != nullptr) {
            return fs::path(local_app_data) / rel_path;
        }
        const char* program_data = std::getenv("PROGRAMDATA")
        if (program_data != nullptr) {
            return fs::path(program_data) / rel_path;
        }
        return fs::path("C:\\ProgramData") / rel_path;
#elif __APPLE__
        const char* home = std::getenv("HOME");
        return fs::path(home != nullptr ? home : "/") / "Library" / "Preferences" / rel_path;
#else
#       error Unknown platform!
#endif
    }

    bool config::load() {
        return load_file<config>(get_path(), *this);
    }

    config::match_map config::find_matches(const std::filesystem::path &p) const {
        match_map result;
        for (const auto& e: syncs) {
            const std::filesystem::path& local_path = e.second.local;
            if (!is_subpath(p, local_path)) {
                continue;
            }
            result[std::distance(local_path.begin(), local_path.end())].push_back(&e);
        }
        return result;
    }

    config::match_vec config::find_most_specific_matches(const std::filesystem::path& p) const {
        match_map m = find_matches(p);
        if (m.empty()) {
            return config::match_vec();
        }
        return m.rbegin()->second;
    }

    const char* const config::name = "config";

}