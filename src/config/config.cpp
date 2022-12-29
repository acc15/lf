#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>

#include "io/serialization.hpp"
#include "io/log.hpp"

#include "config/config.hpp"
#include "config/config_parser.hpp"

namespace fs = std::filesystem;

namespace lf {

    bool check_config_path_specified(const std::string& name, const std::filesystem::path& p, const char* key) {
        if (p.empty()) {
            log.error() && log() << "sync \"" << name << "\" " << key << " path isn't specified" << std::endl;
            return false;
        }
        return true;
    }

    bool check_config_path_absolute(const std::string& name, const std::filesystem::path& p, const char* key) {
        if (!check_config_path_specified(name, p, key)) {
            return false;
        }
        if (!p.is_absolute()) {
            log.error() && log() << "sync \"" << name << "\" " << key << " path must be absolute" << std::endl;
            return false;
        }
        return true;
    }

    std::istream& operator>>(std::istream& s, with_format<format::TEXT, config&> dest) {
        config& cfg = dest.value;
        cfg.syncs.clear();
        
        parse_config(s, [&cfg](const config_entry& e) {
            if (e.section.empty()) {
                log.warn() && log() << "empty section at line " << e.line << std::endl;
                return;
            }

            if (e.value.empty()) {
                log.warn() && log() << "empty value for key \"" << e.key << "\" at line " << e.line << std::endl;
                return;
            }

            config_sync& sync = cfg.syncs[e.section];
            if (e.key == "local") {
                sync.local = e.value;
            } else if (e.key == "remote") {
                sync.remote = e.value;
            } else if (e.key == "index") {
                sync.index = e.value;
            } else if (e.key == "state") {
                sync.state = e.value;
            } else {
                log.warn() && log() << "invalid key \"" << e.key << "\" at line " << e.line << std::endl;
            }
        });

        auto it = cfg.syncs.begin();
        while (it != cfg.syncs.end()) {
            config_sync& sync = it->second;
            
            bool valid = check_config_path_absolute(it->first, sync.local, "local");
            valid &= check_config_path_absolute(it->first, sync.remote, "remote");
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
                it = cfg.syncs.erase(it);
            }
            
        }
        return s;
    }
    
    fs::path config::get_path() {
        const char* config_path = std::getenv(env_name);
        if (config_path != nullptr) {
            return config_path;
        }
        return get_default_path();
    }

    std::filesystem::path config::get_default_path() {
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

    config config::load() {
        const fs::path path = get_path();
        config cfg = load_file<config>(path);
        if (cfg.syncs.empty()) {
            throw std::runtime_error((std::ostringstream() 
                << "config file at " << path << " doesn't have any valid sync entry"
            ).str());
        }
        return cfg;
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
            log.error() && log() << "no configured syncs found for path " << p << std::endl;
            return config::match_vec();
        }
        return m.rbegin()->second;
    }

    const char* const config::name = "config";
    const char* const config::env_name = "LF_CONFIG";

}