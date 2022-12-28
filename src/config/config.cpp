#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>

#include "config.hpp"
#include "config_parser.hpp"
#include "../io/log.hpp"

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
        cfg.clear();
        
        parse_config(s, [&cfg](const config_entry& e) {
            if (e.section.empty()) {
                log.error() && log() << "empty section at line " << e.line << std::endl;
                return;
            }
            if (e.value.empty()) {
                log.error() && log() << "empty value for key " << e.key << " at line " << e.line << std::endl;
                return;
            }

            config_sync& sync = cfg[e.section];
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

        auto it = cfg.begin();
        while (it != cfg.end()) {
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
                it = cfg.erase(it);
            }
        }

        if (cfg.empty()) {
            log.error() && log() << "config doesn't have any valid sync entry" << std::endl;
            s.setstate(std::istream::failbit);
        }
        return s;
    }
    
    fs::path get_config_path() {
        const char* config_path = std::getenv("LF_CONFIG");
        if (config_path != nullptr) {
            return config_path;
        }

        const fs::path rel_path = "lf/lf.yaml";

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
#endif
    }

    const char* const config_desc::name = "config";

}